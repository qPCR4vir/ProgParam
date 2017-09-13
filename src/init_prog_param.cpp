/**
* ProgParam - manage subprograms & programs parameters:
*             definitions & I/O from projects files or UI
* Copyright (C) 2013-2015, Ariel Vina Rodriguez ( arielvina@yahoo.es )
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
*  @file  ProgParam\src\init_prog_param.cpp
*  @brief
*  @autor Ariel Vina-Rodriguez (qPCR4vir)
*/

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


bool    CProject::load()
{   streamoff i=0;
    string    etiq ;    
    ifstream  isPr( _ProjetFileName); 
    if (!isPr) 
        throw std::ios_base::failure(string("Could not open project file: ")+_ProjetFileName );
        
    //    return false;     ///  \todo: en realidad hay que trabajar con las exepciones aqui !!!!!!!!!!!!!!!!!
   // isPr.exceptions(ifstream::failbit | ifstream::badbit);   // no eofbit: http://www.cplusplus.com/reference/iostream/ios/exceptions/
    isPr >> skipws ;

    do {    i=isPr.tellg();        // not good !!!
            //etiq ;                
            getline (isPr, etiq) ;    
            if (string::npos == etiq.find(":") ) 
                continue;
            isPr.seekg(i);

            if (!getline(isPr, etiq, ':'))                // hace falta??? execption ???
                return true;
    //        isPr >> skipws ;
            etiq=trim_string(etiq) ;
            if ( ! load(etiq, isPr))             
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


