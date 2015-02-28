#ifndef _INIT_PROG_PARAM_H
#define _INIT_PROG_PARAM_H
#include "init_prog_param_impl.h"

namespace Programs{
template <typename Param_t>  /// Param_t have to be copiable, comparable and stream <<, >> 
    class CParam: public IParam
{    Param_t  _v, &_value;
 public:
    /// Link to an existing parameter. Do not use the internal _v. For compatibility.
    CParam (        IProg        *pp,  
              const std::string  &titel, 
              const std::string  &etiq, 
                    Param_t      &parRef,  
		      const Param_t      &defValue
			) 
            : IParam (pp, titel, etiq), _value(parRef)     {  _value= defValue;	      }
 
	/// Use the internal parameter and dont need an external one
    CParam (        IProg        *pp,  
              const std::string  &titel, 
              const std::string  &etiq, 
		      const Param_t      &defValue
			) 
            : CParamString (pp,  titel, etiq, _v,  defValue	)  {            }

	void    set(const Param_t& value)     { if (value == _value) return; _value = value;  changed();    }
	Param_t get(                    )const{ return _value;    }

	virtual std::ostream	    &saveValue	(std::ostream	&osPr) const override   
	                        {   return osPr << _value   ;     
	                        } 

	bool        loadValue (std::istream   &isPr) override    
	                    {   
                            Param_t value;
                            isPr >> value;
                            set(value);
							return true;
	                    } 
};


class CParamBool: public IParam
{    bool  _v, &_value;
 public:
                    /// Link to an existing parameter. Do not use the internal   _v. For compatibility.
    CParamBool (IProg *pp,  const std::string& titel, const std::string& etiq, bool &parRef,    
		            bool defValue
					) : IParam (pp, titel, etiq), _value(parRef)            {  _value= defValue;	          }

	               /// Use the internal parameter and dont need an external one
    CParamBool ( IProg              *pp,  
                 const std::string& titel, 
                 const std::string& etiq, 
		         bool               defValue
				) : IParam (pp, titel, etiq), _v(defValue) , _value(_v) {            }

	void set(bool value){ if (value == _value) return; _value = value;  changed();    }
	bool get()const{ return _value;    }

	std::ostream	    &saveValue	(std::ostream	&osPr) const override   
	                        {   osPr << std::boolalpha << _value ;     
	                            return osPr;        // sera solo un problema de IntelliSense ??
	                        } 

	bool        loadValue (std::istream   &isPr) override   // Descarta el Titel que queda detras del ultimo tab
	                    {   
                            bool value; 
                            isPr>>std::boolalpha>>value;
							set( value);
                            return true;
	                    } 
};

class CParamString: public IParam
{    std::string  _v, &_value;
 public:
    /// Link to an existing parameter. Do not use the internal string _v. For compatibility.
    CParamString  (IProg *pp,  const std::string& titel, const std::string& etiq, std::string &parRef,  
		            const std::string& defValue
				  ) : IParam (pp, titel, etiq), _value(parRef)            
                 {  
                    _value= defValue;	          
                 }
	///// Use the internal parameter and dont need an external one
       CParamString (IProg *pp,      const std::string& titel, const std::string& etiq, 
		            const std::string& defValue
					) : IParam (pp, titel, etiq), _v(defValue) , _value(_v) {            }

	/// Use the internal parameter and dont need an external one
    //CParamString (IProg *pp,      const std::string& titel, const std::string& etiq, 
		  //          const std::string& defValue
				//	) : CParamString (pp,  titel, etiq, _v,  defValue	)  {            }

	void set(const std::string& value)
            { 
               if (value == _value) return; 
               _value = value;  
               changed();    
            }
	std::string get()const{ return _value;    }

	virtual std::ostream	    &saveValue	(std::ostream	&osPr) const override   
	                        {   return osPr << _value << std::endl <<"\t\t\t\t" ;     
	                        } 

	bool        loadValue (std::istream   &isPr) override   /// Descarta el Titel que queda detras del ultimo tab
	                    {   std::string t; getline(isPr,t); 
                            t=trim_string(t);
	                        set(trim_string( t.substr(0, t.rfind("\t"))   ));
							return true;
	                    } 
};
class CParamC_str: public IParam, public C_str
{public:
    //CParamC_str (IProg *pp, C_str &parRef,    // Acepta un parametro y por tanto no usa _v. Por compatibilidad.
		  //          const string& titel, const string& etiq, 
		  //          const char * defValue
				//	) : IParam (pp, titel, etiq), _value(parRef)       {  _value.Copy( defValue) ;	   }
    CParamC_str (IProg *pp,                  ///  no necesita un parametro externo
		            const std::string& titel, const std::string& etiq, 
		            const char * defValue
					) : IParam (pp, titel, etiq), C_str(defValue)                {            }
    CParamC_str (IProg *pp,                  ///  no necesita un parametro externo
		            const std::string& titel, const std::string& etiq, 
		            const C_str& defValue
					) : IParam (pp, titel, etiq), C_str(defValue)                {            }

	void set (const C_str& value){ if (! strcmp (Get(),value.Get())) return; Copy(value) ;  changed(); }  // contiene inconsistencias: mas razon para no usarlo.
	void set (const char * value){ if (! strcmp (Get(),value))       return; Copy(value) ;  changed(); }  // contiene inconsistencias: mas razon para no usarlo.
	void trim(const char * value){ if (! strcmp (Get(),value))       return; CopyTrim(value) ;  changed(); }  // contiene inconsistencias: mas razon para no usarlo.
    void take(char * value){ if (! strcmp (Get(),value)){delete[]value;return;} Take(value);changed(); }  // contiene inconsistencias: mas razon para no usarlo.

	std::ostream &saveValue	(std::ostream	&osPr) const override   
	                        {   osPr << Get() << std::endl <<"\t\t\t\t" ;     // Por que no se puede poner el return directo????
	                            return osPr;        // sera solo un problema de IntelliSense ??
	                        } 

	bool        loadValue (std::istream   &isPr) override   // Descarta el Titel que queda detras del ultimo tab
	                    {   std::string t; std::getline(isPr,t); 
							t = trim_string(t);
	                        trim( t.substr(0, t.rfind("\t")).c_str()   );
							return true;
	                    } 
};

    /// To be throw
class ParamOutOfNumRange : public OutOfNumRange 
{ public: 
	explicit ParamOutOfNumRange ( const std::string& what_arg ): OutOfNumRange(what_arg ){}
		template<typename Num>
	ParamOutOfNumRange ( const std::string& what_arg, Num invalidValue, NumRang<Num>& NR ) 
		: OutOfNumRange (  what_arg ,  invalidValue,  NR ) 
		{}
};
    /// To be throw
class ParamOutOfEnumRange : public ParamOutOfNumRange 
{ public: 
	explicit ParamOutOfEnumRange ( const std::string& what_arg ): ParamOutOfNumRange(what_arg ){}
		template<typename Num>
	ParamOutOfEnumRange ( const std::string& what_arg, Num invalidValue, NumRang<Num>& NR ) 
		: ParamOutOfNumRange (  what_arg , int(invalidValue),  NumRang<int>(   int( NR.Min() )   ,   int( NR.Max() )    )    )
		{}
		template<typename Num>
	ParamOutOfEnumRange ( const std::string& what_arg, int invalidValue, NumRang<Num>& NR ) 
		: ParamOutOfNumRange (  what_arg , int(invalidValue),  NumRang<int>(   int( NR.Min() )   ,   int( NR.Max() )    )    )
		{}
};	 


           /// Manage a parametr of type Num (a "numeric" type) for with the value have to be in a range defined by min and max. 
           /// Reimplement set and implement loadValue (with check if value is in range and throw)and saveValue.  
template <typename Num>
class CParamNumRange: public CParamBNRange<Num>
{
    
 public:
					/// It accepts a parameter and therefore does not use _v. For compatibility.
    CParamNumRange ( IProg *pp, const std::string& titel, const std::string& etiq, Num &parRef, 
					 Num min, Num max, Num defValue,
					const std::string& unit=""					) 
        : CParamBNRange (pp, titel, etiq, parRef,min,  max,  defValue,unit)
	    { 
            if (!inRang(defValue)) 
			throw ParamOutOfNumRange(std::string("Error contructing parametr: \"")
												+ Titel() 
												+ "\" ("+ Etiq() + ")" + ", trying to set the default value "
										 ,defValue , *this  );
	    }
								/// Num &parRef,   _v used and therefore does not need an external parameter
    CParamNumRange (IProg *pp, const std::string& titel, const std::string& etiq, 
						Num min, Num max, Num defValue,
						const std::string& unit=""
					) : CParamBNRange (pp, titel, etiq,min,  max,  defValue,unit)
	{ if (!inRang(defValue)) 
		throw ParamOutOfNumRange(std::string("Error contructing parametr: \"")
											+ Titel() 
											+ "\" ("+ Etiq() + ")" + ", trying to set the default value " ,
									defValue , *this  );
	}
	std::ostream	&saveValue	(std::ostream	&osPr) const override   
	                        {return osPr<<get();} 
    bool        loadValue   (std::istream   &isPr) /*throw( ParamOutOfNumRange) */  override         
	                        {   Num t; 
								isPr>>t; 
	                            set(t);
								return true;
	                        } 
	void set(Num value){ if (!inRang(value)) 
		                    throw ParamOutOfNumRange(std::string("Value out of Range while trying to modify: \"")
												     + Titel() 
												     + "\" ("+ Etiq() + ")"
													 , value, *this );
						  CParamBNRange::set(value) ; 
	                    }

};

template <typename enumType>
class CParamEnumRange: public CParamBNRange<enumType>
{
   std::map<std::string, enumType>   _StrValues;
   std::map<   int     , enumType>	 _IntValues;

 public:
	 void AddEnumValues(                         enumType eTy )		{ _IntValues[int(eTy)]=eTy; }
	 void AddStrValues (const std::string& strV, enumType eTy )		{ _StrValues[strV]    =eTy;
                                                                      AddEnumValues(eTy );      }
     const std::map<std::string, enumType>&  StrValues()const{return _StrValues;}

	 std::string ToString(enumType v)const
		{ for (auto p : _StrValues) 
			if (p.second==v)
				return p.first;
		  throw OutOfNumRange(std::string("Value out of Range while trying to set: ")+Titel() );
		}
	 std::string StringEnumerate()const 
	 {	std::ostringstream result;
		for (auto p : _StrValues)
			result <<  p.first << "(" << int(p.second) <<"), ";
		return result.str();
	 }
	 std::string ToString()const {return ToString(get());}
	 bool exist(int v)	        const{		return _IntValues.end()!=_IntValues.find(v); }
	 bool exist(std::string v)	const{		return _StrValues.end()!=_StrValues.find(v); }


								/// It accepts a parameter and therefore does not use _v. For compatibility.
	 CParamEnumRange (IProg *pp, const std::string& titel, const std::string& etiq, enumType &parRef, 
						enumType min, enumType max, enumType defValue,
						const std::string& unit=""
					 ) : CParamBNRange<enumType> (pp,  titel,  etiq, parRef, min, max, defValue, unit	)
	          { if (!inRang(defValue)) 

			    throw ParamOutOfEnumRange(std::string("Error contructing parametr: \"")
												     + Titel() 
												     + "\" ("+ Etiq() + ")" + ", tryin to set the default value " ,
												defValue , *this  );
	          }									
											// enumType &parRef,   usa _v y por tanto no necesita un parametro externo
	 CParamEnumRange (IProg *pp, const std::string& titel, const std::string& etiq, 
						enumType min, enumType max, enumType defValue,
						const std::string& unit=""
					 ) : CParamBNRange<enumType> (pp,  titel,  etiq, min, max, defValue, unit	)
	          { if (!inRang(defValue)) 
			    throw ParamOutOfEnumRange(std::string("Error contructing parametr: \"")
												     + Titel() 
												     + "\" ("+ Etiq() + ")" + ", tryin to set the default value " ,
												defValue , *this  );
	          }								
	void set(int value){	if (_IntValues.empty())
								set(enumType(value));
							else{
								auto p=_IntValues.find(value);
								if (p==_IntValues.end())	
									throw ParamOutOfEnumRange(std::string("Value out of Range while trying to modify: \"")
												     + Titel() 
												     + "\" ("+ Etiq() + ")" + ", with know values "  + StringEnumerate() ,
												value , *this  );
								CParamBNRange::set(enumType(p->second));
							}
	                    }
	void set(const std::string& value){auto p=_StrValues.find(value);
								if (p==_StrValues.end())	 
									throw ParamOutOfEnumRange(std::string( "Value \"" ) + value + "\" out of Range while trying to modify: \"" 
												     + Titel() 
												     + "\" ("+ Etiq() + ")" + ", with have known values: "  + StringEnumerate()  );
								CParamBNRange::set(p->second);
								set(p->second);
	                    }
	void set(enumType value){ if (!inRang(value))  
									throw ParamOutOfEnumRange(std::string("Value out of Range while trying to modify: \"")
												     + Titel() 
												     + "\" ("+ Etiq() + ")" + ", with know values "  + StringEnumerate() ,
												value , *this  );
							 CParamBNRange::set(value) ; 
	                    }

	std::ostream &saveValue	(std::ostream	&osPr) const override   
	                        {	for (auto p : _StrValues) 
									if (p.second==get())
										return osPr<<p.first;
								return osPr<< int(get());
							} 
    bool        loadValue   (std::istream   &isPr) override         
	                        {   std::string t; 
								 //try
									{	isPr>>t; 
										if (exist(t))
										{	set(t);
											return true;
										}// if(!_StrValues.empty()) return false;
										set (stoi(t));
										return true;
									} //catch (OutOfNumRange) {}
								 return false;
							}
};


           /// Manage a parametr of type NumRang<Num> (a min and max value for some "numeric" type) for with the values have to be in defined ranges. 

template <typename Num>
class CParamNumMinMax: public IBParam
{   NumRang<Num>       _v ;
    CParamNumRange<Num> min, max; 
 public:
 								/// It accepts a parameter and therefore does not use _v. For compatibility.
   CParamNumMinMax (IProg *pp, const std::string& titel, NumRang<Num> &parRef , 
		            const std::string& titelmin, const std::string& etiqmin, Num minmin, Num maxmin, Num defValuemin,
		            const std::string& titelmax, const std::string& etiqmax, Num minmax, Num maxmax, Num defValuemax,
		            const std::string& unit=""
					) : IBParam ( titel), 
					    min(pp, titel+". "+titelmin, etiqmin,  parRef.Min(), minmin, maxmin, defValuemin, unit),
					    max(pp, titel+". "+titelmax, etiqmax,  parRef.Max(), minmax, maxmax, defValuemax, unit)
	          { 
	          }
    CParamNumMinMax (IProg *pp, const std::string& titel, //CNumRang<Num> &parRef ,//   usa _v y por tanto no necesita un parametro externo
		            const std::string& titelmin, const std::string& etiqmin, Num minmin, Num maxmin, Num defValuemin,
		            const std::string& titelmax, const std::string& etiqmax, Num minmax, Num maxmax, Num defValuemax,
		            const std::string& unit=""
					) :    CParamNumMinMax ( pp,  titel,  _v , 
		                                     titelmin,   etiqmin,   minmin,   maxmin,   defValuemin,
		                                     titelmax,   etiqmax,   minmax,   maxmax,   defValuemax,
		                                     unit )
					//IBParam (titel), 
					//    min(pp, titel+". "+titelmin, etiqmin,  _v.Min(), minmin, maxmin, defValuemin, unit),
					//    max(pp, titel+". "+titelmax, etiqmax,  _v.Max(), minmax, maxmax, defValuemax, unit)
	          { 
	          }
    CParamNumRange<Num> &Min(){ return min;}
    CParamNumRange<Num> &Max(){ return max;}
    NumRang<Num>   get()const { return _v; }
    NumRang<Num>&  getRef()   { return _v; }
};
}
using namespace Programs ;

#endif


