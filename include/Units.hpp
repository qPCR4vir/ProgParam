/**
* ProgParam - manage subprograms & programs parameters:
*             definitions & I/O from projects files or UI
* Copyright (C) 2013-2016, Ariel Vina Rodriguez ( arielvina@yahoo.es )
*
*    This program is free software : you can redistribute it and / or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.If not, see <http://www.gnu.org/licenses/>.
*
*  @file  Run-time implementation of the concept of units and conversion betwen units or relations between different magnitudes, like molarity and mass concentration for an specific substance.
*  @detail This small library was designed for convenience: easy of use and of extention, not for high performance. 
*   For almost cero-runtime overhead and compile-time detection of errors you may consider boost unit or similar, 
*   for which this library is NOT a replacement of any kind.
*  @autor Ariel Vina-Rodriguez (qPCR4vir)
*/

#ifndef _Units_HPP
#define _Units_HPP

#include <iostream> 
#include <map>
#include <set>
#include <functional>
#include <sstream>
#include <assert.h>
#include <exception>

namespace RTunits  ///< Run-Time defined magnitude untis, convertions, quantities and relations
{ 

	class CUnit;

	using unit_name          = std::string;
	using magnitude_name     = std::string;
	using magnitude_t        = std::set<unit_name>;
	using units              = std::map<unit_name,      CUnit      >;
	using magnitudes         = std::map<magnitude_name, magnitude_t>;
	using nonLinealFunction  = std::function <double(double)>;

	inline bool  unit_exist(const unit_name& n) noexcept;
	inline bool  compatible(const unit_name& name_, const unit_name& base_);
	/// mainly for debuging.
	std::ostream& operator<<(std::ostream& o, const CUnit& u);


struct UnitError :  std::runtime_error
{
	static bool use_throw; ///< def { true }; use carefully - it is a global variable !!

	UnitError(const std::string& what_arg)
		:std::runtime_error{ std::string("Run-time Unit Error: ") + what_arg }
	{}

	UnitError(const char * what_arg)
		:UnitError{ std::string(what_arg)  }
	{}

	void emit()
	{
		if (use_throw)
			throw UnitError(*this);
		std::cerr << what();
	}
};

/// Converting Units (not necessarily linearly), representing the definition of the unit which are all registered.
class CUnit
{
  public:   
    /// An option between a linear or a general function to actually perform the conversion
    struct conversion
    {
        double              c{ 1.0 }, s{ 0.0 };  ///<   u2 = c*u1 + s
        bool                linear{ true };      ///<  \deprecate ?
        nonLinealFunction   nlc{ _identity } ;   ///< arbitrary non linear conversion \todo use to flag linear? 

	
        conversion() noexcept	                            ///< an identity conversion
        {};
        
		conversion(double c_  , double s_=0.0) noexcept   	///< a linear conversion     
			:c(c_),s(s_)
        {};

		/// a general conversion - first apply linear and then nlc \todo conversion( const nonLinealFunction& nlc_ )   ??
        conversion(double c_, double s_,const nonLinealFunction& nlc_) noexcept 
			:c(c_),s(s_), nlc(nlc_)  , linear(false) 
        {};

		/// concatene two arbitrary convertions
        conversion operator*(const conversion& rc) const 
        { 
            if (linear && rc.linear)
                return conversion (rc.c*c, c*rc.s+s);
            if (linear )
                return conversion (rc.c*c, c*rc.s+s, rc.nlc); /// first apply linear and then nlc?
            if (rc.linear )
                return conversion (c,s, [rc,this](double b){return nlc(rc.c*b+rc.s);});

            return conversion (c,s, [rc,this](double b){return nlc(rc.c*rc.nlc(b)+rc.s);});
        }

		/// transform THIS conversion into the inverse, but throw if nonlinear
        conversion invert(bool *error_flag=nullptr)  /// \todo add error code !
        { 
			if (!linear)
			{
				if (error_flag)
					*error_flag = true;
				UnitError("Invertion of non linear function is not possible").emit();
				return *this;
			}
			if (!c)
			{
				if (error_flag)
					*error_flag = true;
				UnitError("Invertion of non dependend quantity is not possible").emit();
				return *this;
			}

			c =1/c;     /// y=cx+s  => x=(1/c)y-s/c
            s=-s*c;     /// c'=1/c  ;  s'= -s/c = -sc'
			if (error_flag)
				*error_flag = false;

            return *this;
        }

		/// return a new inverted conversion, but throw if nonlinear
        conversion inverted(bool *error_flag = nullptr) const
        { 
			return conversion (*this).invert(error_flag);
        }

		/// execute the conversion - return the corresponding value
        double operator()(double ori_val) const 
        {
            if (linear)
                return c*ori_val+s;
            return c*nlc(ori_val)+s;
        }
    } ;

    conversion      conv ;
    unit_name       name, base=unit_name{name};
    magnitude_name  magnitude ;
    bool            error{true};   ///< \todo keep ? or throw?

    /// Create and DEFINE a new conversion: define a "new" "name" unit from another, the base, adding self to a general list of conversions and if needed add new units and even the magnitude.
    CUnit( const unit_name& name_,  
           const conversion& conv_,   
           const unit_name& base_,   
           magnitude_name magnitude_="")
		: name(name_), base(base_), magnitude(magnitude_), conv(conv_)
    {
        add();
    }

	/// Create and DEFINE a new conversion: -the same but for "proportional" units
    CUnit(  const unit_name& name_,  
            double           k_   ,   
            const unit_name& base_,   
            magnitude_name   magnitude_=""  )    
		: CUnit(name_, conversion(k_), base_, magnitude_)
    {
    }

    CUnit()
	{}

    /// create a Conversion from=name, to=base using existing information from others, previosly defined conversions
    CUnit(const unit_name& from, const unit_name& to)   
        : name(from), base(to), error(true)
    {
		if (!unit_exist(to) || !unit_exist(from))
		{
			UnitError(std::string("We need two existing units in order to find a conversion. ") + from + " based on " + to + "\n ").emit();
			return;
		}

		if ( !compatible(from, to) )
		{
			UnitError(std::string("Units ") + from + " (" + _Units[from].magnitude + ") and "
                               + to + " (" + _Units[to].magnitude + ") are not compatible. ").emit();
			return;
		}

		magnitude = _Units[from].magnitude;
        if (from==to)       
        {     
            error=false;     
            return ;       // the conv was defaultly initialize to be 1.
        }
        

        ///  ...--------- from ----------------->

        unit_name  c_from, c_to;           //conv=_Units[from].conv;

        for (c_from=from, c_to=_Units[from].base;              // current from and to taken from the first unit and it base
                            c_from != c_to;                    // upps, tupik in this direction no more convertions, dont walk more
                                c_to=_Units[c_from=c_to].base) // walk to the next convertion
        {
            conv = _Units[c_from].conv * conv;       //  This order is very important
            if (c_to == to)                          // We arrived !! we have U(from,to)
            {     
                error=false;     
                return ;               ///  ...--------- from --------- to ---------...
            } 
            //conv = conv * _Units[c_from].conv;
        }           ///  ...--------- from ----------------basicUnit1
        conversion c2bu1{conv};       // direct Conversion to a basic unit: U( from , c_to = basic unit 1)
        unit_name  bu1{c_to};


        ///  ...--------- to ----------------->

        conv=conversion(); //_Units[to].conv;

        for ( c_from=to, c_to=_Units[to].base;          // current from and to taken from the first unit and it base
                               c_from != c_to;          // upps, tupik in this direction no more convertions, dont walk more
                c_to=_Units[c_from=c_to].base  )        // walk to the next convertion
        {
            if (! conv.linear )   /// \todo what if nonlinear? find the inverse definition?
                return;            
            conv = _Units[c_from].conv * conv;      /// This order is very important
            if (c_to == from)       // We arrived !! we have U(to,from) inverted
            {     
                error=false; 
                conv.invert(&error);
                return ;               ///  ...--------- to --------- from ---------...
            } 
            //onv = conv * _Units[c_from].conv;
        } ///  ...--------- to ----------------basicUnit2


        if (c_to!=bu1)
		{
			UnitError(std::string("Units ") + from + " and " + to + " (" + _Units[from].magnitude + ") have no conversion defined. " ).emit();
		    return;
		}
        if (! conv.linear )   /// \todo what if nonlinear? find the inverse definition?
            return;        

        conv=c2bu1*conv.inverted(&error);    /// \todo This order is very important?? is correct??  TEST !!!!
    }

	/// dont do anything but prepare for future use, remembering some reference unit
    CUnit(const unit_name& from):CUnit(from,from){}

	/// return another convertion, from our present "origen" to "onother base", or just to another unit
    CUnit to(const unit_name& to)
    {
        return CUnit(name,to);
    }

	/// return another convertion, to our present "origen" from "onother base", or just to another unit
    CUnit from(const unit_name& to)
    {
        return CUnit(to,name);
    }

	std::string       to_string() const
	{
		std::ostringstream o;
		o << *this;
		return o.str();
	}

	friend inline const magnitudes& MagnitudesDic() noexcept;
	friend inline const units     & UnitsDic() noexcept;

    static const double No;   // ? No=6.02e23;
	static const char grC[];  // ? = u8"蚓"; 

    static bool Initialize()
    {
        CUnit("m"       , 1     , ""    ,             "Length"      );
        CUnit("km"      , 1000  , "m"                               );
        CUnit("dm"      , .1    , "m"                               );
        CUnit("cm"      , .1    , "dm"                              );
        CUnit("mm"      , .1    , "cm"                              );
        CUnit("L"       , 1     , ""    ,              "Volumen"    );
        CUnit("L"       , 1000  , "mL"                              );
        CUnit("mL"      , 1000  , u8"無"                            );
        CUnit("m3"      , 1000  , "L"                               );
        CUnit("dm3"     , 1     , "L"                               );
        CUnit("cm3"     , 1     , "mL"                              );
        CUnit("mm3"     , 1     , u8"無"                            );
        //CUnit("mm3"     , 1     , "無"                              );  // may provoque redefinition !! just comment it
        CUnit("s"       , 1     , "s"    ,             "Time"       );
        CUnit("s"       , 1000  , "ms"                              );
        CUnit("min"     , 60    , "s"                               );
        CUnit("h"       , 60    , "min"                             );
        CUnit("day"     , 24    , "h"                               );
        CUnit("week"    , 7     , "day"                             );
        CUnit("year"    , 365   , "day"                             );
        CUnit("g"       , 0.001 , "kg"   ,             "Mass"       );
        CUnit("g"       , 1000  , "mg"                              );
        CUnit("mg"      , 1000  , u8"痢"                            );
        CUnit(u8"痢"    , 1000  , "ng"                              );
        CUnit("ng"      , 1000  , "pg"                              );
        CUnit("pg"      , 1000  , "fg"                              );
        CUnit("tone"    , 1000  , "kg"                              );
        CUnit("N"       , 1     , "kg*m*s-2",          "force"      );
        CUnit("J"       , 1     , "kg*m2*s-2",         "energy"     );
        CUnit("J"       , 1     , "N*m"      ,         "energy"     );   // double check energy
        CUnit("cal"     , 4.184 , "J"                               );
        CUnit("kcal"    , 1000  , "cal"                             );
        CUnit("Wh"      , 3600  , "J"                               );
        CUnit("cal/mol" , 4.184 , "J/mol",             "intr_energy");
        CUnit("kcal/mol", 1000  , "cal/mol"                         );
        CUnit("kJ/mol"  , 1000  , "J/mol"                           );
        CUnit("cd"      , 1     , ""     ,             "Luminosity" );
        CUnit("K"       , 1     , ""     ,             "Temperature");
        CUnit(u8"蚓",conversion(1, 273.15), "K"                     );
        CUnit(u8"蚓",conversion(1.8, 32),  u8"蚌"                   );
        //CUnit(u8"蚓"    , 1     , "grC"                             );   // why dont work ????
        CUnit("A"       , 1     , ""     ,             "Current"    );
        CUnit("mA"      , 0.001 , "A"                               );
        CUnit("bp"      , 1     , "nt"    ,             "GeneLength");
        CUnit("kb"      , 1000  , "bp"    ,             "GeneLength");
        CUnit("mol"     , 1     , ""     ,              "Amount"    );   // Amount of substance
        CUnit("mol"     , No    , "cop"                             );
        CUnit("mmol"    , 0.001 , "mol"                             );
        CUnit(u8"痠ol"  , 0.001 , "mmol"                            );
        CUnit("nmol"    , 0.001 , u8"痠ol"                          );
        CUnit("pmol"    , 0.001 , "nmol"                            );
        CUnit("fmol"    , 0.001 , "pmol"                            );
        CUnit("amol"    , 0.001 , "fmol"                            );
        CUnit("zmol"    , 0.001 , "amol"                            );
        CUnit("ymol"    , 0.001 , "zmol"                            );
        CUnit("molecule", 1     , "cop"                             );
        CUnit("particle", 1     , "cop"                             );
        //CUnit("M"       , 1     , ""    ,             "molarity"  );
        CUnit("M"       , No    , "cop/L" ,             "molarity"  );
        CUnit(u8"cop/無"  , 1000  , "cop/L"                         );
        CUnit("M"       , 1     , "mol/L"                           );
        CUnit("M"       , 1000  , "mM"                              );
        CUnit("mM"      , 1000  , u8"然"                            );
        CUnit(u8"然"      , 1     , u8"痠ol/L"                      );
        CUnit(u8"然"      , 1000  , "nM"                            );
        CUnit("nM"      , 1000  , "pM"                              );
        CUnit("pM"      , 1000  , "fM"                              );
      //CUnit("pM"      , 1     , u8"痠ol/L"                        );
        CUnit(u8"痠ol/L"  , 1     , u8"pmol/無"                     );
        CUnit("mg/L"    , 1000  , "g/L" ,            "concentration");
        CUnit(u8"痢/L"    , 1000  , "mg/L"                          );
        CUnit("g/mL"    , 1000  , "g/L"                             );
        CUnit(u8"pg/無"   , 1     , u8"痢/L"                        );
        CUnit("rad"     , 1     , "m/m"    ,          "angle"       );  // Dimensionless units
        CUnit("x"       , 1     , ""    ,             "fraction"    );
        CUnit("x"       , 100   , "%"                               );
        CUnit("sr"      , 1     , "m2/m2"  ,          "solidangle"  );
        CUnit("Hz"      , 1     , "s-1"    ,          "frequency"   );
        CUnit(""        , 1     , ""       ,         "dimensionless");  // will work?

        return true;
    }

  protected:
	  /// units of the same magnitude are compatible, but may be still (yet) non convertible , or the convertion may be unknow \todo make public?

  private:
	  static units                _Units;
	  static magnitudes           _Magnitudes;
	  static nonLinealFunction    _identity;      ///<  ??
	  static bool                 _initialized;   ///<  ??
	  void add();
};


inline const magnitudes& MagnitudesDic() noexcept { return CUnit::_Magnitudes; }
inline const units     & UnitsDic() noexcept { return CUnit::_Units; }

inline bool  unit_exist(const unit_name& n) noexcept
{
	return UnitsDic().find(n) != UnitsDic().end();
}

inline bool  magnitude_exist(const unit_name& m) noexcept
{
	return MagnitudesDic().find(m) != MagnitudesDic().end();
}

/// will throw if one of the unit don't exist
inline bool  compatible(const unit_name& name_, const unit_name& base_)
{
	return UnitsDic().at(name_).magnitude == UnitsDic().at(base_).magnitude;
}



class Relation : public CUnit
{
public:
    Relation(const unit_name& name_,  double   k_            ,   const unit_name& base_ )
        /*: CUnit (name_,base_)*/
    {
        if ( error =  !unit_exist(name_) || !unit_exist(base_))
            return;
        //if ( error = _compatible (name_ , base_ ) && !CUnit (name_,base_).error ) 
        //    return;
        conv = conversion (k_);
        name=name_; base= base_ ;
    }
    Relation(const unit_name& name_,  const conversion& conv_,     const unit_name& base_ )
        /*: CUnit (name_,base_)*/
    {
        if ( error =  !unit_exist(name_) || !unit_exist(base_))
            return;
        //if ( error = _compatible (name_ , base_ ) && !CUnit (name_,base_).error ) 
        //    return;
        conv = conv_;
        name=name_; base= base_ ;
    }
    //Relation(const unit_name& name_,  const conversion& conv_,     const unit_name& base_ )
    //    : CUnit (name_,base_)
    //{
    //    conv = conv_;
    //    error = error ? !unit_exist(name) || !unit_exist(base) || _compatible (name , base ) : true;
    //}
    CUnit operator()(const unit_name& name_, const unit_name& base_)
    {
        CUnit u;
        u.name=name_; u.base=base_ ;
        if (!unit_exist (name_ ) || !unit_exist (base_ ))
            return u;
        //if ( _compatible(name_, base_) )
        //{
        //    u =CUnit(name_, base_);
        //    if (!u.error)
        //        return u;
        //} 
        if ( compatible(name_, name) && compatible(base,base_) )
        {
            CUnit l(name_, name), r(base,base_);
            if (!l.error && !r.error )
            {  
               u.conv=l.conv * conv * r.conv ;
               u.error = false;
               return u;
            }
        }
        if ( compatible(name_, base ) && compatible(name ,base_) )
        {
            CUnit rl(name_, base ), rr(name ,base_);
            if (!rl.error && !rr.error )
            {  
               u.conv=rl.conv * conv.inverted( &(u.error) )  * rr.conv ;
               
            }
        }
        return u;
    }
};
class MW 
{
  Relation m,c;
public:
    MW(double mw):m("mol",mw,"g"),c("M",mw,"g/L"){}
    CUnit operator()(const unit_name& name_, const unit_name& base_)
    {
        CUnit r=m(name_,base_);
        if (!r.error)
            return r;
        return c(name_,base_);
    }
};

struct quantity
{
	double     value;
	unit_name  unit;

	quantity(double value, unit_name  unit) : value{ value }, unit{ unit }
	{
		if (!unit_exist(unit))
			throw UnitError(unit + " - this is not the name of a defined unit: error during quantity definition");
	}

	void convert_into(unit_name  new_unit)
	{
		value = CUnit(unit, new_unit).conv(value);
		unit = new_unit;
	}

	double value_in(unit_name  other_unit) const
	{
		return CUnit(unit, other_unit).conv(value);
	}

	quantity get_in(unit_name  other_unit) const
	{
		quantity re{ *this };
		re.convert_into(other_unit);
		return re;
	}
};

} // namespace Units


#endif 


