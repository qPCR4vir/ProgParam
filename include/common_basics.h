/**
* ProgParam - manage subprograms & programs parameters:
*             definitions & I/O from projects files or UI
* available at: https://github.com/qPCR4vir/ProgParam
*
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
*  @file ProgParam\include\common_basics.h
*  @author Ariel Vina-Rodriguez (qPCR4vir)
*  @brief Basic functionalities. To be used by UI and programs. 
*         Primary, with no dependencies. 
*/


#ifndef _COMMON_BASIC_H
#define _COMMON_BASIC_H

#pragma unmanaged	
#pragma warning( disable : 4996 )

#include <cctype> 
#include <string>
#include <cstring>
#include <sstream>
#include <stdexcept>


/// \deprecated use string and string_view
inline char *clone_c_str   (const char *str)
{return strcpy (new char[std::strlen(str) +1] , str);}

/// elimina espacios al principio y al final \deprecated use string and string_view
inline char *clone_trim_str(const char *str)
{	if(str && str[0])
	{							// llegado aqui - tiene al menos un char !
		size_t  f, i ;
		for ( i=0; str[i] && isspace(str[i]); ++i );   // salta espacios al principio
		for ( f=i; str[f]					; ++f );	// f=Len >0. llega al final (considerando que char(0) es el final !!!!! )
		for ( --f;  f>=i  && isspace(str[f]); --f );			// se come espacios al final
		size_t NewLen= f - i+1 ;

		if (NewLen>0)					// strncpy(NewStr, &str[i], NewLen) ; 		
		{
			char *NewStr= new char[NewLen+1];
			NewStr[NewLen]=0;
			while ( NewLen )
				NewStr[--NewLen]=str[f--];
			return NewStr ;
		}
	}
									// vacia o solo espacios. O sea: no char dist de espacio
	char *NewStr= new char[1]; 
	NewStr[0]=0;
	return NewStr;
}

///   Elimina espacios al principio y al final \deprecated use string and string_view
template<typename CharType>
inline CharType *clone_trim(const CharType *str)		// definida en    :   init_prog_param.cpp   -  elimina espacios al principio y al final
													//char *clone_trim_str(const char *str)
{	if(str && str[0]) 
	{							// llegado aqui - tiene al menos un char !
		size_t  f, i ;
		for ( i=0; str[i] && isspace(str[i]); ++i );   // salta espacios al principio
		for ( f=i; str[f]					; ++f );	// f=Len >0. llega al final (considerando que char(0) es el final !!!!! )
		for ( --f;  f>=i  && isspace(str[f]); --f );			// se come espacios al final
		size_t NewLen= f - i+1 ;

		if (NewLen>0)					// strncpy(NewStr, &str[i], NewLen) ; 		
		{
			char *NewStr= new CharType[NewLen+1];
			NewStr[NewLen]=0;
			while ( NewLen )
				NewStr[--NewLen]=str[f--];
			return NewStr ;
		}
	}
									// vacia o solo espacios. O sea: no char dist de espacio
	char *NewStr= new CharType[1]; 
	NewStr[0]=CharType(0);
	return NewStr;
}


template<class _Elem,	class _Traits,	class _Alloc>               //   elimina espacios al principio y al final
inline std::basic_string<_Elem, _Traits, _Alloc> trim_string(const std::basic_string<_Elem, _Traits, _Alloc>& str)		
{	size_t i,l=str.length() ;
	if (l)
	{
		for ( i=0;  i<l  && isspace(static_cast<unsigned char>(str[i])); ++i )
			;           // salta espacios al principio
		for ( --l; l>=i  && isspace(static_cast<unsigned char>(str[i])); --l )
			;			// se come espacios al final
		size_t NewLen= l - i +1;
		if (NewLen>0)					// strncpy(NewStr, &str[i], NewLen) ; 		
			return str.substr(i,NewLen);
	}
	return std::basic_string<_Elem, _Traits, _Alloc>();
}

///  Clase para lidiar con las string como file name y sec que deben ser copiadas y liberadas todo el tiempo. Copy() vs Take() !!!!  \deprecated use string and string_view
class C_str
{public:
/*explicit*/ C_str()													:_s(clone_c_str		("")){}
	explicit C_str(const char * s)										:_s(clone_c_str		(s )){}
	explicit C_str(const C_str& s)										:_s(clone_c_str		(s._s )){}  // Copy constructor


	char *Copy		(const char *s)	{ if (s!=_s) delete []_s; _s=clone_c_str	(s );				return _s		;}  // return correct ???
	C_str& Copy		(const C_str& s){ if (this!= &s) Copy(s._s);									return *this	;}	// "sinonimos"
	C_str& operator=(const C_str& s){ 																return Copy(s)	;}	// = return Copy(s) ;					
	char *CopyTrim	(const char *s)	{ if (s!=_s) delete []_s; _s=clone_trim_str	(s );				return _s		;}
	char *Trim		(			)	{ char *t=_s;  			_s=clone_trim_str	(_s); delete []t;	return _s		;}
	char *Take		(	   char *s) { if(s!=_s) delete []_s;										return _s=	(s );}
	char *TakeTrim	(	   char *s) { Take(s);														return Trim()	;}
	char *Get		()				{		/* PELIGROSO !!!! use release or Copy ?? */				return _s		;}
	const char *Get	()		const	{																return _s		;}
	char *release	()				{  char *t=_s;  	_s=clone_c_str		("")	;				return t		;}


	~C_str			()				{ delete []_s; }
private:
	char *_s;
};

/// Numeric Range 
template <class Num>
class NumRang				// ------------------------------------------------------------------- NumRang<Numeric>	-----------------
{	
    Num min{}, max{} ; 
public: 
	NumRang(					  )                            {} 
    explicit NumRang(Num av		  ):min(av),			max(av){}
	NumRang(Num mi,         Num ma):min(mi),			max(ma){}

	void Set(					   ){}
	void Set(		Num av		   ){min=av			; max =av  ;}
	void Set(Num mi,         Num ma){min=mi			; max =ma  ;}
	void Set(     NumRang    R     ){min=R.min		; max=R.max;}

	void SetMin(Num mi			   ){min=mi;			    }
	void SetMax(			 Num ma){				max=ma ;}

	Num  Ave() const {return (min+max)/2 ;}				// Lineal aritmetical Average "by default"
	Num  Min() const {return min			;}			Num& Min()		 {return min			;}
	Num  Max() const {return max			;}			Num& Max()       {return max			;}
   	Num length()const{ return Max() - Min() ;}          // throw execption "negative length ??????


	bool inRang  ( Num		X)   const {return (min<= X			&& X<=max)		;}
    bool isIntern( Num		X)   const {return (min<  X			&& X< max)		;}
	bool inRang  ( NumRang	R)   const {return ( R.inRang(min) && R.inRang(max));}
	bool isExtrem( Num		X)   const {return (min== X			|| X==max)		;}
	bool isExtrem( NumRang	R)   const {return (min== R.Min()	|| R.Max()==max);}

	void Expand(Num num)			 { if ( num<min ) min=num  ; if(  num>max)  max=num  ;}
	void Expand(Num mi,       Num ma){ if (   mi<min) min=mi   ; if(   ma>max)  max=ma   ;}
	bool Expand(NumRang R)			 { bool e; if (e=(R.min<min)) min=R.min; if(R.max>max)  {max=R.max;return true;} return e;}
    void Shrink(NumRang R)			 { if (R.min>min) min=R.min; if(R.max<max)  max=R.max;}
	Num  Intersection(NumRang R){return R.Shrink(this);}	//Num  Union(NumRang R)	    {return R.Expand(this);}


	bool UpdateMin(Num mi){ return (mi<min) ? min=mi, true : false ;}
	bool UpdateMax(Num ma){ return (ma>max) ? max=ma, true : false ;}
};

/// Numeric Range with settable Average
template <class Num>
class NumRangA	: public NumRang<Num>	// -------------------------------------- NumRangA<Numeric> 	--------------
{public: 
	NumRangA(					   ):					ave(0)				{} 
explicit NumRangA(	 Num av		   ):NumRang<Num>(av),		ave(av)				{}
	NumRangA(Num mi,         Num ma):NumRang<Num>(mi,ma),	ave(NumRang<Num>::Ave()) {}
	NumRangA(Num mi, Num av, Num ma):NumRang<Num>(mi,ma),	ave(av)				{}

	void Set(					   ){NumRang<Num>::Set(  )   ;ave=0				;}
	void Set(		Num av		   ){NumRang<Num>::Set(av)   ;ave=av				;}
	void Set(Num mi,         Num ma){NumRang<Num>::Set(mi,ma);ave=NumRang<Num>::Ave() ;}
	void Set(Num mi, Num av, Num ma){NumRang<Num>::Set(mi,ma);ave=av				;}

	void SetAve(     Num av        ){	    ave=av;					 }

	Num  Ave() const {return ave ;}		Num&  Ave()  {return ave ;}

private: 
	Num ave; 
};




template <class Num>
inline	bool inRang(const Num X, const NumRang<Num> R)  {return  R.inRang(X) ;}


class DegRes			//---------------------------------------------------	DegRes	-------------------------------------
{public: float min,ave,max; 
	DegRes(							   ):min(0 ),ave(0 )		,max(0 ){}; 
    explicit 	DegRes(	float av	   ):min(av),ave(av)        ,max(av){};
	DegRes(float mi,           float ma):min(mi),ave((mi+ma)/2) ,max(ma){};
	DegRes(float mi, float av, float ma):min(mi),ave(av)        ,max(ma){};

	void Set(float mi, float av, float ma){min=mi; ave=av       ; max=ma;}
	void Set(float mi,           float ma){min=mi; ave=(mi+ma)/2; max=ma;}
	void Set(          float av          ){min=av; ave=av       ; max=av;}
	void Set(                            ){        ave=(min+max)/2      ;}
};


		template <typename Num>
 std::string toString_Val_in_Range(Num val, const NumRang<Num> &NR)
{     
	std::ostringstream result;
	result << val << " in range [" << NR.Min() <<", "<< NR.Max() << "]";
	return result.str();
}
		template <typename Num>
 std::string toString_Range(NumRang<Num> &NR)
{     
	std::ostringstream result;
	result << " [" << NR.Min() <<", "<< NR.Max() << "]";
	return result.str();
}




class OutOfNumRange : public std::out_of_range 
{ public: 
	explicit OutOfNumRange ( const std::string& what_arg ) : out_of_range(what_arg + " (Out of numeric rang)" ){}

		template<typename Num>
	explicit OutOfNumRange ( const std::string& what_arg, Num invalidValue, const NumRang<Num>& NR )
		: out_of_range(what_arg + " (Out of numeric rang, trying to set " + toString_Val_in_Range(invalidValue, NR) + ")"   ) 
		{}
};


//class OutOfNumRangeT : public std::out_of_range 
//{ public: 
//	explicit OutOfNumRangeT ( const std::string& what_arg ) : out_of_range(what_arg + " (Out of numeric rang)" ){}
//
//		template<typename Num>
//	explicit OutOfNumRangeT ( const std::string& what_arg, Num invalidValue, NumRang<Num>& NR ) 
//		: out_of_range(what_arg + " (Out of numeric rang, trying to set " + toString_Val_in_Range(invalidValue, NR) + ")"   ) 
//		{}
//};


#endif 