/**
* ProgParam - manage subprograms & programs parameters:
*             definitions & I/O from projects files or UI
* Copyright (C) 2013-2018, Ariel Vina Rodriguez ( arielvina@yahoo.es )
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
*  @file  ProgParam\include\init_prog_param.h
*  @brief  
*  @autor Ariel Vina-Rodriguez (qPCR4vir)
*/

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
			: IParam (pp, titel, etiq), _v(defValue) , _value(_v) {            }

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

/// deprecated. Use CParamString
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
	ParamOutOfNumRange ( const std::string& what_arg, Num invalidValue, const NumRang<Num>& NR )
		: OutOfNumRange (  what_arg ,  invalidValue,  NR ) 
		{}
};
    /// To be throw
class ParamOutOfEnumRange : public ParamOutOfNumRange 
{ public: 
	explicit ParamOutOfEnumRange ( const std::string& what_arg ): ParamOutOfNumRange(what_arg ){}
		template<typename Num>
	ParamOutOfEnumRange ( const std::string& what_arg, Num invalidValue, const NumRang<Num>& NR )
		: ParamOutOfNumRange (  what_arg , int(invalidValue),  NumRang<int>(   int( NR.Min() )   ,   int( NR.Max() )    )    )
		{}
		template<typename Num>
	ParamOutOfEnumRange ( const std::string& what_arg, int invalidValue, const NumRang<Num>& NR )
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
        : CParamBNRange<Num> (pp, titel, etiq, parRef,min,  max,  defValue,unit)
	    { 
            if (!CParamBNRange<Num>::inRang(defValue))
			throw ParamOutOfNumRange(std::string("Error contructing parametr: \"")
												+ CParamBNRange<Num>::Titel()
												+ "\" ("+ CParamBNRange<Num>::Etiq() + ")" + ", trying to set the default value "
										 ,defValue , *this  );
	    }
								/// Num &parRef,   _v used and therefore does not need an external parameter
    CParamNumRange (IProg *pp, const std::string& titel, const std::string& etiq, 
						Num min, Num max, Num defValue,
						const std::string& unit=""
					) : CParamBNRange<Num> (pp, titel, etiq,min,  max,  defValue,unit)
	{ if (!CParamBNRange<Num>::inRang(defValue))
		throw ParamOutOfNumRange(std::string("Error contructing parametr: \"")
											+ CParamBNRange<Num>::Titel()
											+ "\" ("+ CParamBNRange<Num>::Etiq() + ")" + ", trying to set the default value " ,
									defValue , *this  );
	}
	std::ostream	&saveValue	(std::ostream	&osPr) const override   
	                        {return osPr<<CParamBNRange<Num>::get();}
    bool        loadValue   (std::istream   &isPr) /*throw( ParamOutOfNumRange) */  override         
	                        {   Num t; 
								isPr>>t; 
	                            set(t);
								return true;
	                        } 
	void set(Num value){ if (!CParamBNRange<Num>::inRang(value))
		                    throw ParamOutOfNumRange(std::string("Value out of Range while trying to modify: \"")
												     + CParamBNRange<Num>::Titel()
												     + "\" ("+ CParamBNRange<Num>::Etiq() + ")"
													 , value, *this );
						  CParamBNRange<Num>::set(value) ;
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
		  throw OutOfNumRange(std::string("Value out of Range while trying to set: ")+CParamBNRange<enumType>::Titel() );
		}
	 std::string StringEnumerate()const 
	 {	std::ostringstream result;
		for (auto p : _StrValues)
			result <<  p.first << "(" << int(p.second) <<"), ";
		return result.str();
	 }
	 std::string ToString()const {return ToString(CParamBNRange<enumType>::get());}
	 bool exist(int v)	        const{		return _IntValues.end()!=_IntValues.find(v); }
	 bool exist(std::string v)	const{		return _StrValues.end()!=_StrValues.find(v); }


								/// It accepts a parameter and therefore does not use _v. For compatibility.
	 CParamEnumRange (IProg *pp, const std::string& titel, const std::string& etiq, enumType &parRef, 
						enumType min, enumType max, enumType defValue,
						const std::string& unit=""
					 ) : CParamBNRange<enumType> (pp,  titel,  etiq, parRef, min, max, defValue, unit	)
	          { if (!CParamBNRange<enumType>::inRang(defValue))

			    throw ParamOutOfEnumRange(std::string("Error contructing parametr: \"")
												     + CParamBNRange<enumType>::Titel()
												     + "\" ("+ CParamBNRange<enumType>::Etiq() + ")" + ", tryin to set the default value " ,
												defValue , *this  );
	          }									
											// enumType &parRef,   usa _v y por tanto no necesita un parametro externo
	 CParamEnumRange (IProg *pp, const std::string& titel, const std::string& etiq, 
						enumType min, enumType max, enumType defValue,
						const std::string& unit=""
					 ) : CParamBNRange<enumType> (pp,  titel,  etiq, min, max, defValue, unit	)
	          { if (!inRang(defValue)) 
			    throw ParamOutOfEnumRange(std::string("Error contructing parametr: \"")
												     + CParamBNRange<enumType>::Titel()
												     + "\" ("+ CParamBNRange<enumType>::Etiq() + ")" + ", tryin to set the default value " ,
												defValue , *this  );
	          }								
	void set(int value){	if (_IntValues.empty())
								set(enumType(value));
							else{
								auto p=_IntValues.find(value);
								if (p==_IntValues.end())	
									throw ParamOutOfEnumRange(std::string("Value out of Range while trying to modify: \"")
												     + CParamBNRange<enumType>::Titel()
												     + "\" ("+ CParamBNRange<enumType>::Etiq() + ")" + ", with know values "  + StringEnumerate() ,
												value , *this  );
								CParamBNRange<enumType>::set(enumType(p->second));
							}
	                    }
	void set(const std::string& value){auto p=_StrValues.find(value);
								if (p==_StrValues.end())	 
									throw ParamOutOfEnumRange(std::string( "Value \"" ) + value + "\" out of Range while trying to modify: \"" 
												     + CParamBNRange<enumType>::Titel()
												     + "\" ("+ CParamBNRange<enumType>::Etiq() + ")" + ", with have known values: "  + StringEnumerate()  );
		                        CParamBNRange<enumType>::set(p->second);
								set(p->second);
	                    }
	void set(enumType value){ if (!CParamBNRange<enumType>::inRang(value))
									throw ParamOutOfEnumRange(std::string("Value out of Range while trying to modify: \"")
												     + CParamBNRange<enumType>::Titel()
												     + "\" ("+ CParamBNRange<enumType>::Etiq() + ")" + ", with know values "  + StringEnumerate() ,
												value , *this  );
		                            CParamBNRange<enumType>::set(value) ;
	                    }

	std::ostream &saveValue	(std::ostream	&osPr) const override   
	                        {	for (auto p : _StrValues) 
									if (p.second==CParamBNRange<enumType>::get())
										return osPr<<p.first;
								return osPr<< int(CParamBNRange<enumType>::get());
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


//typedef CEspProg *pCEspProgParam ;
/// Clase base para los parametros "Especificos" de programas "Especificos".
/// derivar para concretar parametros comunes. Mantiene link a proj de los prog Espec que los usan.
///
//How to use?
// Each parameter have an unique identificator or etiquette.
// While loading, the text between the beginning of a line and the first : will be taken as
// an etiquette (discarding surrounding but not internal spaces).
//If the etiquette is known (valid), the rest of the line will be use to deduce the value of the parameter.
//Some parameter (like names of files) will assume this rest-line-text entirely as his valid value.
//For such parameter please, add any comment in the next line.
//Others parameter (like numeric or bool parameters) will only use the beginning of this rest-line-text
//and will ignore the end.
//Any line without a valid etiquette will be ignore (they are comments!).�
//Only the last valid value of each parameter will be used.
//For not defined parameters, the previous value (from the previously active project
//or from the program�s default) will be use.
//Send questions please to: ArielVina.Rodriguez@fli.bund.de

	class CProject : public IProg
	{
		std::string		    _defPr ;
		std::string		    _ProjectFileName ;
		std::vector<IProg*> _ProgList;
	public:
		CProject(std::string titel, std::string	prFname="", std::string	defProFN="Def.Proj.txt")
				:   IProg           (titel),
					_defPr          (defProFN)  ,
					_ProjectFileName (prFname.empty () ? _defPr :  prFname )
		{
		}

		~CProject()override { }

		void	    ProjectFile	(const std::string &ProjetFileName){	_ProjectFileName=trim_string(ProjetFileName);	}
		std::string ProjectFile	(                            )const{	return _ProjectFileName;	}

		bool load		();
		bool load_defPr	()                                  { ProjectFile(_defPr);         return load();	}
		bool load	    (const std::string &ProjectFileName){ ProjectFile(ProjectFileName); return load();	}

		///  \todo  Este es el verdadero save !!! El que abre el fichero y lo salva todo.
		void saveToFile	(const char *ProjectFileName) const{	std::ofstream osPr(ProjectFileName);
			save(osPr); }

		void save		()		const		            {   saveToFile(_ProjectFileName.c_str())	;   }
		void save_defPr	()                              {   ProjectFile(_defPr);
			save();	    }
		void save_asDefPr()		const		            {	saveToFile(_defPr.c_str())	        ;   }
		void save	 (const std::string &ProjectFileName){	ProjectFile(ProjectFileName);
			save();	    }

		virtual void saveTMP() const            /// Reescribe el projecto actual. Pensar algo mejor? Preguntar al user? usar # conscuti?
		{	  save();	}


		std::ostream	&save(std::ostream	&osPr)  const override
		{
			for(auto p : _ProgList)
				p->save(osPr) ;		     // save all subprograms
			IProg::save(osPr) ;          // save onw parametr list

			osPr<< std::endl<<std::endl<<
				"How to use? \n Each programs parameter have an unique identificator or etiquette. \n "
				"While loading, the text between the beginning of a line and the first : will be taken as\n "
				"an etiquette (discarding surrounding but not internal spaces). \n"
				"IF the etiquette is know (valid), the rest of the line will be use to deduce the value of the parameter. \n"
				"Some parameter (like file�s names) will assume this rest-line-text entirely as his valid value. \n"
				"For such parameter please, add any comment in the next line. \n"
				"Other parameter (like numeric or bool parameters) will only use the beginning of this rest-line-text and ignore the end. \n"
				"Any line without a valid etiquette will be ignore (they are comments!).� \n"
				"Only the last valid value of each parameter will be used\n"
				"For not defined parameters, the previous value (from the previously active project or from the program�s default) will be use.\n"
				"Direct questions please to ArielVina.Rodriguez@fli.bund.de\n"
					;

			return (osPr) ;

		}   // por que solo funciona con el IProg:: ???
		bool            load    (std::string &etiq, std::istream  &isPr) override
//		bool	            load_all(std::string &etiq, std::ifstream &isPr)	//override
		{
			for(auto p : _ProgList)
				if ( p->load(etiq, isPr))       // load from any subprogram
					return true ;
			return IProg::load(etiq, isPr);		  // load from onw parametr list
		}

		int		Run (IProg &prog)	override                    //   ??????
		{
			saveTMP( ) ;
			return prog.Run();
		}

		void AddProg (IProg* prog) {_ProgList.push_back(prog);}

	};

	class CCommProgParam : public IProg
	{	CProject *_proj;
	public:
		CCommProgParam  (const std::string& titel,       CProject *proj=nullptr)
				: IProg(titel,proj),   _proj(proj) {}
		~CCommProgParam() override	{}

		//std::ostream	&save(std::ostream	&osPr 				 ) const override
		//                    {
		//						assert(("attempt to use an uninitialized project pointer in save_all",_proj));
		//						return _proj->save(osPr);
		//                    }
		//bool	    load(std::string     &etiq, std::istream &isPr) override
		//                    {
		//						assert(("attempt to use an uninitialized project pointer in load_all",_proj));
		//						return _proj->load(etiq,isPr);
		//                    }
		void        AddProgToProject(IProg *p)
		{
			assert(("attempt to use an uninitialized project pointer in AddProgToProject",_proj));
			_proj->AddProg(p);
		}
		virtual std::string  MakeRuningName()const {return "";}

	};

	class	CEspProg  : public IProg
	{public:										// Permite no duplicar los parametros comunes en los parametros especificos
		explicit CEspProg(const std::string& titel, CCommProgParam &commParam )
				: _cp(commParam), IProg(titel )
		{ _cp.AddProgToProject(this);}

		CCommProgParam &_cp;

		std::ostream	&save_with_comm(std::ostream &osPr)	const		   // Save all needed parameters for this program, and not only the specific ones
		{          _cp.save(osPr) ;
			return     IProg::save(osPr) ;    }

		bool	    load_with_comm(std::string &var, std::istream &isPr)	  // Usar estas dos funciones solo si se quiere save or load olny this program
		{ 	if ( _cp.load(var, isPr))       // PAra salvar el projecto completo use el save_all del projecto
				return true ;
			return IProg::load(var, isPr);					 }
	} ;

}
using namespace Programs ;

#endif


