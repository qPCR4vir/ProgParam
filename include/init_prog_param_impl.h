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
*  @file  ProgParam\include\init_prog_param_impl.h
*  @brief
*  @author Ariel Vina-Rodriguez (qPCR4vir)
*/


#ifndef _INIT_PROG_PARAM_IMPL_H
#define _INIT_PROG_PARAM_IMPL_H
#define _CRT_SECURE_NO_WARNINGS
#pragma unmanaged
#pragma warning( disable : 4996 )
#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <time.h>
#include <map>
#include <vector>
#include <functional>

#include "common_basics.h" 

/// \brief Organize a "software" or Project into Specialized Programs and manage the input/config parameters for each of the programs. 
/// Make ease to write a program interface with a command-line, a text "project file" or a GUI, or all of then together. 
/// Definitions and declarations for the user interface. Also to be used directly by the programs. Almost primary, depend only on Common basics.
///  \todo:  PROBLEMA : como organizar estos parametros si usamos procesos? Hacer copia de ellos !!!!!!!!?
namespace Programs{
class IProg ;
/// Non abstract basic Interface of programs parameter which define default save, load and title use.
class IBParam
{
   std::string _Titel;
 public:

   IBParam (  const std::string &titel)      //  pp->?????????????
		    :_Titel(titel) 
	        {         
	        }    
	 virtual ~IBParam(){}

	 std::string Titel   ()const{return _Titel;}               ///<  ???  Human readable
	 void        SetTitel(std::string titel){ _Titel=titel;}   ///<  ???  Human readable


 virtual std::ostream &save	(std::ostream	&osPr) const        ///< The default. To be change in derivate classes
			            {   osPr<< ".\t"<<Titel()<<std::endl; 
							return osPr;
			            } 
 virtual bool   load	(std::istream   &isPr)  ///< throw( std::out_of_range)                
			            {   return false;}   
 virtual bool   load	(std::string &etiq, std::istream &isPr) ///<  throw( std::out_of_range) 
			            {   return false;}   

};
   // ifstream& operator >>(ifstream& ifs,IParam& p);                             //    ?????????????????
   // ofstream& operator <<(ofstream& ofs,const IParam& p){ p.save(ofs); return ofs;};  //    ?????????????????

/// Base class to manage a parameter of "some" type (defined in derived classes).  

/// It will call a IParam::ValueChanged callback function and set IParam::Unit and IParam::Etiq. 
/// Used to save to and load from an stream, without implementing the load and IParam::saveValue functions. 
class IParam : public IBParam
{    
    std::string _etiq, _unit;
 protected: 
	void changed()
	        {   
                if(ValueChanged) 
				    ValueChanged(/**this*/);
	        }

 public:
     std::function<void(void/*IBParam& param*/)> ValueChanged ; ///< A callback each time the value change

	 IParam (  IProg *pp,                     ///< the program who own the parameter (typically "this", the object for which the parameter is a member) 
               const std::string& titel, 
               const std::string& etiquete, 
               const std::string& unit="" ) ;

	 std::string Etiq(           )const{return _etiq;}      ///< semiHuman readable and unique. I use with length 10
	 void     SetEtiq(std::string etiq, IProg *prog);
	
	 std::string Unit()const{return _unit;}                 ///< Human readable and optional

	 std::ostream	&save	(std::ostream	&osPr) const override  ///< Implemented using IParam::saveValuee plus a call to IBParam::save 
			            {    
                            osPr<< _etiq << ":\t"; 
			                saveValue(osPr)<<"\t"<<_unit<<"\t";
							IBParam::save(osPr)	;              ///< << ".\t"<<Titel()<<std::endl; 
							return osPr;
			            } 
     bool       load	(std::istream   &isPr)    override   ///< Asume etiquete allready tested OK !!!!   throw( std::out_of_range).  
			            {   
                            return loadValue(isPr);}   
	 bool       load	(std::string		&etiq, std::istream &isPr)  override  ///<   throw( std::out_of_range).  
			            {   
                            if (etiq!=_etiq) 
						        return false;
			                return load(isPr);
			            }  
                                            /// Default behavior, not yet a real implementation 
	virtual std::ostream	    &saveValue	(std::ostream	&osPr	) const  ///<  =0;   ??No salva nada, no tiene "value" todavia
	                                {return osPr;} 
                                            /// Default behavior, not yet a real implementation 
    virtual bool        loadValue	(std::istream   &isPr)  ///<   throw( std::out_of_range).  
	                                {return false;}

	  ~IParam()override{}
};

           /// Only partially manage a parametr of type Num (a "numeric" type) for with the value have to be in a range defined by min and max. 
 
           /// Implement get and set (with check if value is in range and throw) but NOT loadValue and saveValue. Do not check the DefValue. 
  template <typename Num>
class CParamBNRange: public IParam, public NumRang<Num>
{
    Num  _v, &_value;
 public:
								/// It accepts a parameter and therefore does not use _v. For compatibility.
    CParamBNRange (IProg *pp, const std::string& titel, const std::string& etiq, Num &parRef, 
						Num min, Num max, Num defValue,
						const std::string& unit=""
					) : IParam (pp, titel, etiq, unit), 
					    NumRang<Num>(min,max), 
						_value(parRef)
	          { /*if (!inRang(defValue)) 
			        throw ParamOutOfNumRange(string("Default Value out of Range while trying to construct: ")+Titel() ); */
	             _value=defValue ;
	          }

								/// Num &parRef,   _v used and therefore does not need an external parameter
    CParamBNRange (IProg *pp, const std::string& titel, const std::string& etiq, 
						Num min, Num max, Num defValue,
						const std::string& unit=""
					) : CParamBNRange ( pp,  titel,  etiq, _v, min,  max,  defValue, unit )  
	          {  
	          }
	void set(Num value){ if (!NumRang<Num>::inRang(value)) ///  \todo: Incluir value y rang que no concuerdan en mensaje to throw
		                    throw OutOfNumRange(std::string("Value out of Range while trying to set: ")+Titel(), value, *this );
	                     if (_value==value) return; 
						 _value=value ; 
						 changed();
	                    }
	Num get()const{return _value;
    }
	//virtual ostream	    &saveValue	(ostream	&osPr	) const  // =0;   ??No salva nada, no tiene "value" todavia
};


class CProject;
//class CEspProg ;

/**  \brief   Abstract "Interface" for parameters of programs which only save/load project and run programs (all virtual)
 *
 *   \todo Para crear y anadir un nuevo programa:
 *		- General plan: crear (plan) interfase de usuario para tener idea de los parametros a usar
 *		- Common Programm Parametrs: crear class tomando como base Programs::CCommProgParam o uno de sus derivados para contener todos los parametros comunes
 *		- Specific programs: Para cada tarea crear class tomando como base CEspProg o uno de sus derivados para contener los parametros especificos
 *      - Project: crear class tomando como base Programs::CProject y anadirle todos los Programs::IProg anteriores
 *      - Parametrs: Anadir a cada uno de los anteriores Programs::IProg los parametr especif (como se vio en la UI) e inicializarlos 
 *      - New Parametrs: Si es necesario implementar derivado de Programs::IParam:
 *		    - implementar funciones load/save del project. Load: con etiqueta para cada param, "	<< boolalpha " para bool, 
 *		    - implementar funciones de actualizar parametros <=> interfase de usuario : UpdateThDyP() & UpdateThDyForm()
*		- crear funcion del programa en su propio .cpp que toma como argumento un puntero a class C, call it in a overloaded Programs::IProg::Run 
 */
class IProg : public IBParam // -------	  Clase    ----------
{   
 public:
    std::map<std::string,IParam*> _parametrs;
    void insertParam(IParam *pp){_parametrs[pp->Etiq ()]=pp;}

	IProg (const std::string& titel, CProject *proj=nullptr); /*:_Titel(titel){ if (proj) proj->_ProgList.push_back(this);}*/
	std::ostream	&save		(std::ostream	&osPr				 )  const override
	                            {   
                                    osPr << "\n\t------\t"<<Titel()<<" \n" ;
									for (auto &par : _parametrs) 
								        par.second->save(osPr); 
	                                return osPr;
	                            }
	         bool	load		(std::string		&etiq, std::istream &isPr) override
	                            {   
                                    auto p=_parametrs.find(etiq); 
	                                if (p==_parametrs.end()) 
										return false;
	                                return p->second->load(isPr); //throw exception if false ????
	                            }
	virtual	int		Run			(IProg &prog				){return prog.Run();}       //  ???????
	virtual int		Run			(		void					)
	                            {   
                                    for(int WorkToDo=Initialize(); WorkToDo>0 ; WorkToDo=Continue()) 
										CallBack(WorkToDo); 
									return Finalize();			 
	                            } 
	virtual int		Initialize	(		void					){ return 0;} 
	virtual int		Continue	(		void					){ return false;}
	virtual int		Finalize	(		void					){ return 0;} 
	virtual void	CallBack	(		int WorkToDo			){}
	virtual ~IProg() override{}
};	

}
//using namespace Programs ;
#endif


