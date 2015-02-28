#ifndef ProgParamGUIBind_H
#define ProgParamGUIBind_H

#include <memory>
#include <vector>
 
//#include "ThDySec\matrix.h" 
#include "common_basics.h" 
#include "init_prog_param.h" 

namespace ParamGUIBind 
{
                            /// from the user code (the "Program") or the GUI layout?
enum  class PriorizeDefault {Form, Parametr, NoDef};          

class IParBind	
{	
	static  PriorizeDefault _def;   /*=PriorizeDefault::Form;*/	
  public: 
    virtual void    UpDateProg()=0, UpDateForm()=0 ;	
    virtual ~IParBind()    {}
    static void SetDef(PriorizeDefault def){_def=def;}
    void SetDef(void)
    {  switch (_def)
       {
           case PriorizeDefault::Form:			UpDateProg();	break;		
	       case PriorizeDefault::Parametr:		UpDateForm();	break;
	       case PriorizeDefault::NoDef:						
           default:	;	
       }
    }				
};
typedef std::unique_ptr<IParBind> upPbind;

                 /// A container that will be a member of a form, and will collect all the other ParBind
class BindGroup : public IParBind 
{ private: 	
   std::vector<upPbind>  _pb ;

  public:		
     BindGroup&	operator<<(upPbind&& pb){  add(std::move(pb)) ;	  return *this;	 }
     void              add(upPbind&& pb){  _pb.push_back(std::move(pb))   ;}
     ~BindGroup();

	void	UpDateForm(	 )	override { for(upPbind& pb : _pb)	pb->UpDateForm(	 );}
	void	UpDateProg(	 )	override { for(upPbind& pb : _pb)	pb->UpDateProg(	 );}
};

class ProgPBind : public virtual IParBind 
{
  protected:
      IParam& _p;
  public:
      ProgPBind(IParam& p):_p(p)
      {
          _p.ValueChanged =[&](){UpDateForm();
          };
      }
};

class Bind_CParamBool : public ProgPBind  
{ 	
 public:				
    Bind_CParamBool (CParamBool &p ):ProgPBind(p){} 

    void updateProg(bool val){ static_cast <CParamBool& >(_p).set  (val); }
    bool getProgVal(){ return  static_cast <CParamBool& >(_p).get  () ; }
};

class Bind_CParamString : public ProgPBind  
{ 	
 public:				
    Bind_CParamString (CParamString &p ):ProgPBind(p){} 

    void        updateProg(const std::string&  val){ 
        static_cast <CParamString& >(_p).set (val); }
    std::string getProgVal(               ){ 
        return  static_cast <CParamString& >(_p).get (  ) ; }
};
class Bind_CParamC_str : public ProgPBind  
{ 	
 public:				
    Bind_CParamC_str (CParamC_str &p ):ProgPBind(p){} 

    void        updateProg(const char*  val){ 
        static_cast <CParamC_str& >(_p).Copy     (val); 
    }
    const char* getProgVal(        ){ 
        return  static_cast <CParamC_str& >(_p).Get      (  ) ; 
    }
};
class Bind_CParamC_str_TRIM : public Bind_CParamC_str  
{ 	
 public:				
    Bind_CParamC_str_TRIM (CParamC_str &p ):Bind_CParamC_str(p){} 

    void        updateProg(const char*  val){ static_cast <CParamC_str& >(_p).CopyTrim (val); }
};


template <class Num>
class Bind_CParamRang  : public ProgPBind  
{ 	
 public:				
    Bind_CParamRang (CParamNumRange<Num> &p ):ProgPBind(p){} 

    void updateProg (Num val) { static_cast <CParamNumRange<Num>& >(_p).set  (val); 
                              }
    Num  getProgVal (){ return  static_cast <CParamNumRange<Num>& >(_p).get  () ; 
                      }
};

template <typename enumType>
class Bind_CParamEnumRange: public ProgPBind
{ 	
 public:				
    Bind_CParamEnumRange (CParamEnumRange<enumType> &p ):ProgPBind(p){} 

    void updateProg (const std::string& val) { static_cast <CParamEnumRange<enumType>& >(_p).set  (val); 
                              }
    std::string  getProgVal (){ return  static_cast <CParamEnumRange<enumType>& >(_p).ToString  () ; 
                      }
};

}
#endif


//template <class Num>
//class Bind_CParamRang_Min  : public ProgPBind //    Bind a Control.CheckBox with a bool variable ---- TagBinding_bool    :
//{ 	
// public:				
//    Bind_CParamRang_Min (CParamNumMinMax &p ):ProgPBind(p){} 
//
//    void updateProg(bool val){ static_cast <CParamBool&         >(_p).set    (val); }
//    bool getProgVal(){ return  static_cast <CParamBool&         >(_p).get    () ; }
//};
