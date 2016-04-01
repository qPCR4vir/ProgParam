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
*  @file  ProgParam\scr\Units.cpp
*  @brief
*  @autor Ariel Vina-Rodriguez (qPCR4vir)
*/

#include <Units.hpp>
#include <iostream> 


namespace RTunits
{

bool UnitError::use_throw{ true };
//bool UnitError::use_throw{ false };


double ide(double ori){return ori;}

CUnit::nonLinealFunction    CUnit::_identity(ide);
CUnit::units                CUnit::_Units ;
CUnit::magnitudes           CUnit::_Magnitudes;
bool                        CUnit::_initialized = CUnit::Initialize();
const double                CUnit::No=6.02e23;


std::ostream& operator<<( std::ostream& o, const CUnit& u)
{
   o << "Unit: "<< u.name << " = "<< u.conv.c << " "  ;

   if (! u.conv.linear)        o << " x nonlineal function of " ;

   o <<u.base;

   if (u.conv.s)              o << " + " << u.conv.s ;

   o << "    " << u.magnitude ;

   return o;
}

void CUnit::add()
{
    if(name=="")
    {    
		throw UnitError(to_string()+ "\n We need a new unit name in order to create a new unit. ");
        return;
    }
    if (unit_exist(name))         // this Unit name allready exist
    {    
        if (unit_exist(base))     // this base allready exist too
			return  UnitError(to_string() + "\n " + name + "was allready defined as: " + _Units[name].to_string()).emit();
        
		// the unit exist, but the base dont: asume we are actualy defining the base

        if (! conv.linear) 
			return  UnitError(to_string() + "\nUnable to define " + base + " by reverting the non-lineal definition of "+ name ).emit();

		conv.invert(&error); 
		if (error)
			return;

        name.swap(base);           
    } 
	
	error = true;

    if (unit_exist(base))   //  base already exist
    {
        if(_Units[base].magnitude != magnitude)          
            if (magnitude=="")
                magnitude=_Units[base].magnitude;    // definimos la nueva unidad en la magnitud de la base
            else 
				return  UnitError(to_string() + "\nThe new unit and the base need to be of the same magnitude. " +
                                 base + " was allready defined as: \n" +  _Units[base].to_string() ).emit();
            
		    //  normal !!

    }   else                                // La base no existe
        if (magnitude=="")
			return  UnitError(to_string() + "\nWe need a new magnitude name in order to create the new base unit: " + name ).emit();
        else
            if(base=="")
            { 
                base=name;
                conv=conversion(1);
            } else 
                if (base!=name)  
                    CUnit(base,1,base,magnitude);// Create first the base, as base

        // then the new Unit
    error=false ;
    _Units[name]=*this;
    _Magnitudes[magnitude].insert(name);

}

} // namespace Units
