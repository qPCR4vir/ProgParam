//#include "StdAfx.h"
#pragma unmanaged
#include "init_prog_param.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>

#include <iomanip>
using namespace std;

//const 	EnergyRang        G_def   (-5,-1);
//const 	TemperatureRang   Tm_def  (57,63) ;
//const 	SecPosRang       L_def   (20,35) ;		/*_sL(G_def, Tm_def, L_def),*/
//  void insertParam(IProg *pp) const override;


IParam::IParam (  IProg *pp, 
                  const std::string& titel, 
                  const std::string& etiq, 
                  const std::string& unit ) 
		    : IBParam(titel), 
			  _etiq( etiq==""? Titel() : etiq ),  /// si no quieres introducir una etiq puedes usar el Titel !!
              _unit( unit )
	        {  
				assert (pp); 
				pp->insertParam(this); 
	        }    

void     IParam::SetEtiq(std::string etiq, IProg *prog)
{ 
    auto param = prog->_parametrs[_etiq];     /// \todo revisar posibles inconsistencias   !!!!!!!!!!!!!!!!
    prog->_parametrs.erase(_etiq);
    _etiq=etiq;
    prog->_parametrs[_etiq]=param;
}       ///< Human readable


//void IParam::insertParam(IProg *pp)  
//{   pp->_parametrs[_etiq]= this;
//}

IProg::IProg (const string& titel, CProject *proj) // CProject *proj=nullptr)
	:IBParam( titel)
{ 
	if (proj) proj->AddProg (this);
}


bool	CProject::load()
{   streamoff i=0;
	string etiq ;	
	ifstream isPr( _ProjetFileName); 
	if (!isPr) 
		throw std::ios_base::failure(string("Could not open project file: ")+_ProjetFileName );
		
	//	return false;	 ///  \todo: en realidad hay que trabajar con las exepciones aqui !!!!!!!!!!!!!!!!!
   // isPr.exceptions(ifstream::failbit | ifstream::badbit);   // no eofbit: http://www.cplusplus.com/reference/iostream/ios/exceptions/
	isPr >> skipws ;

	do {	i=isPr.tellg();		// not good !!!
			//etiq ;			    
			getline (isPr, etiq) ;	
			if (string::npos == etiq.find(":") ) 
				continue;
			isPr.seekg(i);

			if (!getline(isPr, etiq, ':'))                // hace falta??? execption ???
				return true;
	//        isPr >> skipws ;
			etiq=trim_string(etiq) ;
			if ( ! load_all(etiq, isPr))	         
				isPr.ignore(1000,'\n')  ;
		}
	while (isPr.good() ); 
	return true;

    //    std::ifstream f("doesn't exist");
    //try {
    //    f.exceptions(f.failbit);
    //} catch (const std::ios_base::failure& e)
    //{
    //    std::cout << "Caught an ios_base::failure.\n"
    //              << "Explanatory string: " << e.what() << '\n'
    //              << "Error code: " << e.code() << '\n';
    //}
	//if ( ! isPr ) return false; //{cerr << "File "<< _ProjetFileName <<" could not be opened "<<endl ; return false;} 
}



//const long MAXSEC_LENGHT=2000-1 ; // un aproximado de lo que creo acepta mi PC con 4 GB
								  //SecLim (this, "Fragments of seq. to use",	_SecLim, 
										//			"Beginng",								"SecBeginng", long(1), long(1)+MAXSEC_LENGHT, long(1),
										//			"End",									"SecuencEnd", long(1), long(1)+MAXSEC_LENGHT, long(1)+MAXSEC_LENGHT,
										//	"nt"),











//int		microArrayProg ( char *InputPrimer, char *InputTarget, char *OutputTm) // se apropia de los punteros a FileName's
//{	ThDyCommProgParam cp;
//	CProgParam_microArray  IPrgPar_uArr(cp);
//		IPrgPar_uArr.SetSondeFile ( InputPrimer );
//		IPrgPar_uArr._cp.SetTargetFile( InputTarget  ) ;
//		IPrgPar_uArr._cp.SetOutputFile( OutputTm) ;
//
//		return	IPrgPar_uArr.Run();
//}



