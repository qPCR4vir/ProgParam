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
*  @file  ProgParam\src\init_prog_param.cpp
*  @brief
*  @autor Ariel Vina-Rodriguez (qPCR4vir)
*/

#include <cassert>
#include <iostream>
#include <string>
#include <iomanip>

#include "init_prog_param.h"

IParam::IParam (  IProg *pp, 
                  const std::string& titel, 
                  const std::string& etiq, 
                  const std::string& unit ) 
            : IBParam(titel), 
              _etiq( !etiq.empty() ? etiq : Titel()),  /// si no quieres introducir una etiq puedes usar el Titel !!
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

IProg::IProg (const std::string& titel, CProject *proj) // CProject *proj=nullptr)
    :IBParam( titel)
{ 
    if (proj) proj->AddProg (this);
}


bool    CProject::load()
{
    std::ifstream  isPr( _ProjetFileName);
    if (!isPr) 
        throw std::ios_base::failure("Could not open project file: "+_ProjetFileName );
        
    isPr >> std::skipws ;

    std::string line ;
    while (std::getline (isPr, line))
    {
        std::string    etiq;
        std::istringstream is_line{line};
        if (!std::getline(is_line, etiq, ':').good())  continue;
        etiq=trim_string(etiq) ;
        load(etiq, is_line);
    }
    return true;
}


