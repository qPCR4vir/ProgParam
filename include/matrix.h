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
*  @file  ProgParam\include\matrix.h
*  @brief
*  @autor Ariel Vina-Rodriguez (qPCR4vir)
*/


//   matrix.h     :   La declaracion y la definicion de funciones miembros de template class TIENEN que estar en el mismo file
//   Because templates are compiled when required, this forces a restriction for multi-file projects: 
//   the implementation (definition) of a template class or function must be in the same file as its declaration. 
//   That means that we cannot separate the interface in a separate header file, and that we must include both interface and implementation in any file that uses the templates.

    //Since no code is generated until a template is instantiated when required, 
    //compilers are prepared to allow the inclusion more than once of the same template file with both declarations and definitions in a project without generating linkage errors.



#pragma unmanaged    
#ifndef _MATRIX_H
#define _MATRIX_H
#include "common_basics.h"
#include <assert.h>
#include <vector>
#include <string>

    using index = int ;    

    /// an Array of Rows of Num (RA) - an implementation detail for the class CTable
    template <typename Num>
class CMatrix_RA
{public:
    using index   = ::index;
    using RowType =   Num* ;
private:
    RowType *_mtx{};
    index   _capRow{},                                 ///<   _cap: capacity, reserved memory 
            _numRow{},                                 ///<   _num:used memory
            _capCol{}, _numCol{}, _cr{}, _cc{};   
    bool    Resize        (void);        ///<  --- kill old data. "Compacta", pero destruyendo datos anteriores. ??? Para que??
    void    forceResize   (void);        ///<  --- kill old data        //      Hace falta ??????
    void    Delete        ();            ///<  --- kill old data
    //void    CopyRef       (const CMatrix_RA& mtx);        //  ????
public:
    CMatrix_RA    (                               ) {    }     

               ///    Reserva "suficiente" memoria
    CMatrix_RA    (index CapRow, index CapColumns){            
                                                        Create ( CapRow, CapColumns);    
                                                  }  
              
    void    Create      (index CapRow, index CapColumns);   
    //bool    Reset        (index NumRow, index NumColumns);        //  --- kill old data?

    /// kill old data. Destruye y re-crea.    Reserva "suficiente" memoria
    void    forceResize (index CapRow, index CapColumns){ 
                                                            Delete(); 
                                                            Create (CapRow, CapColumns);    
                                                        }    

     ~CMatrix_RA    ( )        { 
                                   Delete() ;
                               }     

    Num   &operator() (index row, index col    )   { 
                                                            return _mtx[row][col]    ;
                                                    }
    Num    operator() (index row, index col    )const   { 
                                                            return _mtx[row][col]    ;
                                                        }
    Num    &at        (index row, index col    )        { 
                                                           assert(row<totalRow() && col<totalCol()); 
                                                           return _mtx[row][col];    
                                                        }
    Num    at         (index row, index col    )const   { 
                                                           assert(row<totalRow() && col<totalCol()); 
                                                           return _mtx[row][col]    ;
                                                        }

    Num    &expand        (index row, index col    ) ;    ///<   Variante de acceso a elemento (row,col) que amplia zona en uso e incluso capacidad si necesario

            /// aumenta la capacidad de rows A newRowCap rows
    void    Expand2RowsCap(index newRowCap        )     {    
                                                            AddRowsCap    (newRowCap-_capRow ) ;  
                                                        }    
            /// aumenta la cap de rows en: NumRows rows
    void    AddRowsCap    (index NumRows=1        ); 

            /// aumenta el num de rows en: NumRows rows
    RowType    AddRows        (index NumRows=1     )     {                     
                                                            _cr=_numRow; 
                                                            _cc=0; 
                                                            Expand2RowsCap ( _numRow= _numRow + NumRows ); 
                                                            return _mtx[_cr]; 
                                                        }                                   
    index   totalRow    (                    )const    { return _numRow        ;}
    index   totalCol    (                    )const    { return _numCol        ;}
    void    swap        (CMatrix_RA& mtx     );                 ///<   swap - muy economico
    void    compact     () ;                                    ///<   dismunuye cap a zona en uso
    void    Copy        (const CMatrix_RA& mtx  );              ///<   copia contenido, aumentando zona en uso y cap si necesario. Conserva datos propios no reescritos
            CMatrix_RA  (const CMatrix_RA& mtx  ){              ///<   crea copia con cap restringida a zona en uso    
                                                    Create (mtx.totalRow(), mtx.totalCol() ); 
                                                    Copy(mtx);
                                                 }    
    void    SetNext     (index row, index col   )      { 
                                                            assert(row<totalRow() && col<totalCol()); 
                                                            _cr=row ; 
                                                            _cc=col;
                                                        }
    RowType    NextRow      ()                          {    
                                                            _cc=0; return _mtx[++_cr];
                                                        }
    Num        &Next        ()                          {    
                                                            return expand(_cr,_cc++);    // at (_cr,_cc++);  ??????????????
                                                        }
    CMatrix_RA& operator >> (Num &num)        const     {    
                                                            num    = Next(); 
                                                            return *this ;
                                                        }
    CMatrix_RA& operator << (const Num &num)            {    
                                                            Next() = num   ; 
                                                            return *this ;
                                                        }
    void    InitializeRow(index row, const Num& num) 
                                                        { 
                                                            for (index c=0; c<totalCol();c++) 
                                                               operator()(row,c)=num;
                                                        }
    void    InitializeCol(index col, const Num& num)    { 
                                                            for (index r=0; r<totalRow();r++) 
                                                                operator()(r,col)=num;
                                                        }
    void    Initialize   (             const Num& num)  { 
                                                            for (index r=0; r<totalRow();r++)
                                                               for (index c=0, RowType row=Row(r); c<totalCol();c++) 
                                                                   row[c]=num;
                                                        }
    RowType Row(index row)                              { 
                                                            assert(row<totalRow() ); 
                                                            return _mtx[row];
                                                        }


};


        template <typename Num>
class CTable : public CMatrix_RA<Num>
{    
    std::vector<std::string>   _titRows, _titColumns;
    std::string                _titTable ;
public:
    CTable(std::string TitTable)                              : _titTable(TitTable) {}
    CTable(std::string TitTable,index capRow, index capCol): CMatrix_RA<Num>(capRow, capCol),        _titTable(TitTable), 
                                                             _titRows(capRow),                       _titColumns(capCol)
                                                        {    _titRows.reserve (capRow ); _titColumns.reserve(capCol);}
    index AddColummnTit  (const std::string &newColTit) { _titColumns.push_back  (newColTit); return  index(_titColumns.size()); }
    index AddRowTit      (const std::string &newRowTit) { _titRows.push_back     (newRowTit); return  index(_titRows.size());}
    void CreateMatrix    (index capRow, index capCol)
                        {                                                                                         /* assert (!_mtx); */
                            forceResize( capRow > _titRows   .size() ? capRow : _titRows   .size() , 
                                         capCol > _titColumns.size() ? capCol : _titColumns.size()   );                                  
                        }// si predices las posibles dimenciones
    void CreateMatrix    (index capRow)                
                        {                                                                                            /* assert (!_mtx); */
                            forceResize( std::max <index>(capRow, _titRows   .size() )  , static_cast<index>(_titColumns.size()     ));                                 
                        }// si predices las posibles dimenciones
    void        CreateMatrix    (             )                { /*assert (!_mtx);*/ forceResize(_titRows.size(), _titColumns.size() );    }// deduciendo las dimenc a partir de los tit
    index       AddRow     (const std::string &newRowTit)    { AddRowTit(newRowTit); AddRows(); return totalRow()-1;}
    std::string TitColumn  (index  Col) const{    return _titColumns.at(Col);    }
    std::string TitRow     (index  Row) const{    return    _titRows.at(Row);    }
    std::string TitTable   (          ) const{    return    _titTable       ;    }
    void        TitTable   (std::string titel )  {     _titTable=titel      ;    }
//    bool    compact        ();
};


        template <typename Num>                //            Reserva "suficiente" memoria    --------- Create ()  --------
void CMatrix_RA<Num>::Create (index CapRow, index CapColumns)
{    
    try            { _mtx=nullptr ; _mtx=new RowType[CapRow];  _capRow = CapRow ;}
    catch (...)    { /*? Delete();    /*??*/        assert (("No memory for row pointers in matrix"    ,true)); throw; }     // para el futuro de CExeption

    for (index r=0; r<_capRow ; ++r)    _mtx[r]= nullptr;

    try            { for (index r=0; r<_capRow ; ++r)    _mtx[r]=new Num[CapColumns];     _capCol = CapColumns ;}
    catch (...)    { Delete();    /*??*/        assert (("No memory for row  in matrix"            ,true));    throw; } 
    _numRow= _numCol= _cr=_cc=0;
}

        template <typename Num>                                //            compact    --- dismunuye cap a zona en uso
void CMatrix_RA<Num>::compact        () 
{   
    if(_capRow<=totalRow() && _capCol<=totalCol() )  return ;
    if(_capCol<=totalCol() ) 
    {    for (index r=totalRow() ; r<_capRow ; r++ ) 
            delete  []_mtx[r] ; 
        _capRow=totalRow() ;
        return ;
    }
    CMatrix_RA<Num> mtx(*this);
    swap(mtx);
}

        ///          copia contenido, aumentando zona en uso y cap si necesario. Conserva datos propios no reescritos
        template <typename Num>       
void CMatrix_RA<Num>::Copy        (const CMatrix_RA& mtx    )
{    
    if  (  _capRow >= mtx.totalRow()  &&  _capCol >= mtx.totalCol() )
    {   
        for        ( index r=0        ; r<totalRow()    ; ++r) 
        for        ( index c=0        ; c<totalCol()    ; ++c) this->operator()(r,c)= mtx(r,c) ;
        _numRow =  _numRow < mtx.totalRow() ? mtx.totalRow() :  _numRow ;
        _numCol =  _numCol < mtx.totalCol() ? mtx.totalCol() :  _numCol ;
    } else 
        if (_capCol >= mtx.totalCol() )
        {    Expand2RowsCap( mtx.totalCol()    ) ;
            Copy (mtx);
        } else 
            {    forceResize(mtx.totalRow(), mtx.totalCol() ); 
                Copy(mtx);}
}

        template <typename Num>        //                                    swap ---- muy economico
void CMatrix_RA<Num>::swap        (CMatrix_RA& mtx        )
{   RowType    *t_mtx  =mtx._mtx    ;    mtx._mtx   =_mtx    ; _mtx   =t_mtx ;
    index    t_capRow=mtx._capRow;   mtx._capRow=_capRow ; _capRow=t_capRow ;
    index    t_numRow=mtx._numRow;   mtx._numRow=_numRow ; _numRow=t_numRow ;

    index    t_capCol=mtx._capCol;   mtx._capCol=_capCol ; _capCol=t_capCol ;
    index    t_numCol=mtx._numCol;   mtx._numCol=_numCol ; _numCol=t_numCol ;    
}

        template <typename Num>                    //    aumenta la capacidad de rows en: NumRows rows  --- AddRowsCap  ----
void CMatrix_RA<Num>::AddRowsCap    (index NumRow=1            )
{   
    if (NumRow <=0) return;

    index t_capRow=_capRow +NumRow;    
    RowType    *t_mtx=_mtx ;

    try            {  _mtx=new RowType[t_capRow];  }
    catch (...)    {  _mtx=t_mtx ;         throw;  }     // para el futuro de CExeption

    index r;
                        for  ( r=0              ; r<_capRow     ; ++r) _mtx[r] = t_mtx[r] ;
                        for  ( r=_capRow        ; r<t_capRow    ; ++r) _mtx[r] = nullptr;
    try            {    for  ( r=_capRow        ; r<t_capRow    ; ++r) _mtx[r] = new Num[_capCol];     _capRow = t_capRow ;}
    catch (...)    {    for  ( r=_capRow        ; r<t_capRow    ; ++r) delete []_mtx[r] ;     
                        delete []_mtx ;            
                        _mtx=t_mtx;
                        throw;
                   } 
    delete []t_mtx ; 
}

        template <typename Num>        //    Variante de acceso a elemento (row,col) que amplia zona en uso e incluso capacidad si necesario
Num     &CMatrix_RA<Num>::expand        (index row, index col    ) 
{    
    if(row<totalRow() && col<totalCol()    )    // No need to expand  !!!
        return operator()(row,col);   

    if( col<_capCol )                        // No need to expand columns, only expand Rows!!! (row >= totalRow() )==true
    {    
        if( row >= _capRow )
        {    Expand2RowsCap    ( row+1 );    
            _numRow = _capRow ;            
        }

        if( col >= totalCol() )    
            _numCol = col+1;

        return operator()(row,col);
    }

    index t_capRow = row < _capRow ? _capRow : row+1 ;
    CMatrix_RA<Num> t_m(t_capRow,col+1);
    t_m.Copy( *this );
    swap(t_m) ;
    _numCol = col+1; _numRow = t_capRow ;
    return this->operator()(row,col); 
}

        template <typename Num>                        //  --- kill old data
void CMatrix_RA<Num>::Delete        ()
{   
    while (_capRow) 
        delete [](_mtx[--_capRow]) ; 

    delete []_mtx ; 
    _mtx=nullptr; 
    _capCol=0; _numRow=_numCol= _cr=_cc=0;
}

        template <typename Num>
void CMatrix_RA<Num>::forceResize  ()
{        
    forceResize (_numRow,_numCol);
}

        template <typename Num>                    //  --- kill old data  si "Compacta",  Para que??   --------   Resize        ()  --------
bool CMatrix_RA<Num>::Resize        ()
{    
    if ( _numRow < _capRow      ||     _numCol < _capCol )    
    {        
        forceResize (_numRow,_numCol);
        return true ;
    } else    return false;
}


//class CMatrix_f
//{//    typedef int index;    
//    typedef float Num;
//    typedef Num *RowType;
//
//    RowType    *_mtx ;
//    index    _capRow, _numRow, _capCol, _numCol;
//    bool    Resize        ();        // implemented --- kill old data?
//    void    forceResize ();        // implemented --- kill old data    //      Hace falta ??????
//    void    Delete        ();        // implemented --- kill old data
//    void    CopyRef        (const CMatrix_f& mtx);
//public:
//            CMatrix_f    (index CapRow, index CapColumns){ Create ( CapRow, CapColumns);    }    //                             Reserva "suficiente" memoria
//            CMatrix_f    (                                );        // implemented
//            bool    Reset        (index NumRow, index NumColumns);        // implemented --- kill old data?
//    void    forceResize (index CapRow, index CapColumns){ Delete(); Create (CapRow, CapColumns);    }    // kill old data. Destruye y re-crea.    Reserva "suficiente" memoria
//            ~CMatrix_f    (                        )        { Delete()                ;}                    
//    Num        &operator() (index row, index col    )        { return _mtx[row][col]    ;}
//    Num         operator() (index row, index col    )const    { return _mtx[row][col]    ;}
//    Num        &at            (index row, index col    )        { assert(row<totalRow() && col<totalCol()); return _mtx[row][col]    ;}
//    Num         at            (index row, index col    )const    { assert(row<totalRow() && col<totalCol()); return _mtx[row][col]    ;}
//    Num        &expand        (index row, index col    )    ;        //            Variante de acceso a elemento (row,col) que amplia zona en uso e incluso capacidad si necesario    
//    void    Expand2RowsCap    (index newRowCap    )        {    AddRowsCap    (newRowCap-_capRow ) ;  }    // aumenta la capacidad de rows a NumRows rows
//    void    AddRows        (index NumRow=1            )        {    Expand2RowsCap ( _numRow= _numRow + NumRow ) ;}            // aumenta la cant de rows en: NumRows rows
//    void    AddRowsCap    (index newRowCap        )    ;
//    index    totalRow    (                    )const    { return _numRow        ;}
//    index    totalCol    (                    )const    { return _numCol        ;}
//    void    Copy        (const CMatrix_f& mtx    ) ;
//    void    swap        (CMatrix_f& mtx        );
//    void    Create (index CapRow, index CapColumns);//            Reserva "suficiente" memoria
//};
//
//
//
//
//        template <typename Num>
//class CMatrix_CRA
//{public:
////    typedef int index;    
//    class CRow
//    {public:
//        Num    *_row ;
//        CRow(        ): _row( nullptr    ){}
//        CRow(index n): _row( new Num[n]    ){}
//        ~CRow(){delete []_row;}
//        //friend class CMatrix_CRA;
//    };
//private:
//    CRow    *_mtx ;
//    int            _capRow, _numRow, _capCol, _numCol;
//    bool    Resize        ();        // implemented
//    void    forceResize ();        // implemented    //      Hace falta ??????
//    void    Delete        ();        // implemented
//public:
//            CMatrix_CRA    (int CapRow, int CapColumns);        // implemented
//    bool    Use            (int NumRow, int NumColumns);        // implemented
//    void    forceResize (int CapRow, int CapColumns);        // implemented
//            ~CMatrix_CRA (                    )        { Delete()                ;}                    
//    Num        &operator() (int row, int col    )        { return _mtx[row][col]    ;}
//    Num         operator() (int row, int col    )const    { return _mtx[row][col]    ;}
//    Num        &element    (int row, int col    )        { assert(row<totalRow() && col<totalCol()); return _mtx[row][col]    ;}
//    Num         element    (int row, int col    )const    { assert(row<totalRow() && col<totalCol()); return _mtx[row][col]    ;}
//    int        totalRow    (                    )const    { return _numRow        ;}
//    int        totalCol    (                    )const    { return _numCol        ;}
//};

//
//        template <typename Num>
//class CTable
//{    CMatrix_CRA    *_mtx;
//    vector<string> _titRows, _titColums;
//};


//        template <typename Num>
//class CMatrix_RA
//{    
//    
//    typedef Num *pNum;
//    Num        **_mtx ;
//    int            _capRow, _numRow, _capCol, _numCol;
//    bool    Resize        ();        // implemented
//    void    forceResize ();        // implemented
//    void    Delete        ();        // implemented
//public:
//            CMatrix_RA    (int CapRow, int CapColumns);        // implemented
//    bool    Use            (int NumRow, int NumColumns);        // implemented
//    void    forceResize (int CapRow, int CapColumns);        // implemented
//            ~CMatrix_RA (                    )        { Delete()                ;}                    
//    Num        &operator() (int row, int col    )        { return _mtx[row][col]    ;}
//    Num         operator() (int row, int col    )const    { return _mtx[row][col]    ;}
//    Num        &element    (int row, int col    )        { assert(row<totalRow() && col<totalCol()); return _mtx[row][col]    ;}
//    Num         element    (int row, int col    )const    { assert(row<totalRow() && col<totalCol()); return _mtx[row][col]    ;}
//    int        totalRow    (                    )const    { return _numRow        ;}
//    int        totalCol    (                    )const    { return _numCol        ;}
//};



//
//     CMatrix_f::CMatrix_f        () : _capRow(0), _numRow(0), _capCol(0), _numCol(0),    _mtx(nullptr) {    }
//void CMatrix_f::Create            (index CapRow, index CapColumns)//            Reserva "suficiente" memoria    --------- Create ()  --------
//{    
//    try            { _mtx=nullptr ; _mtx=new RowType[CapRow];  _capRow = CapRow ;}
//    catch (...)    { /*? Delete();    /*??*/        assert (("No memmory for row pointers in matrix"    ,true)); throw; }     // para el futuro de CExeption
//
//    for (index r=0; r<_capRow ; ++r)    _mtx[r]= nullptr;
//
//    try            { for (index r=0; r<_capRow ; ++r)    _mtx[r]=new Num[CapColumns];     _capCol = CapColumns ;}
//    catch (...)    { Delete();    /*??*/        assert (("No memmory for row  in matrix"            ,true));    throw; } 
//}
//void CMatrix_f::forceResize        ()                    //     --- kill old data  y "Compacta",  Hacen falta ??????  -----  forceResize  () ---
//{        forceResize (_numRow,_numCol);}
//void CMatrix_f::Delete            ()
//{    while (_capRow) 
//        delete [](_mtx[--_capRow]) ; 
//    delete []_mtx ; 
//    _mtx=nullptr; 
//    _capCol=0; _numRow=_numCol=0;
//}
//void CMatrix_f::AddRowsCap        (index NumRow        )    // aumenta la capacidad de rows en NumRows rows  ----  AddRowsCap (index NumRow    )
//{    if (NumRow <=0) return;
//    index t_capRow=_capRow +NumRow;    
//    RowType    *t_mtx=_mtx ;
//    try            {  _mtx=new RowType[t_capRow];  }
//    catch (...)    {  _mtx=t_mtx ;         throw;  }     // para el futuro de CExeption
//    index r;
//    for                    ( r=0            ; r<_capRow        ; ++r) _mtx[r] = t_mtx[r] ;
//    for                    ( r=_capRow        ; r<t_capRow    ; ++r) _mtx[r] = nullptr;
//    try    {            for    ( r=_capRow        ; r<t_capRow    ; ++r) _mtx[r] = new Num[_capCol];     _capRow = t_capRow ;}
//    catch (...)    {    for ( r=_capRow        ; r<t_capRow    ; ++r) delete []_mtx[r] ;     
//                    delete []_mtx ;            
//                    _mtx=t_mtx;
//                    throw;} 
//    delete []t_mtx ; 
//}
//void CMatrix_f::    swap        (CMatrix_f& mtx        )                                //        swap - muy economico
//{   RowType    *t_mtx  =mtx._mtx    ;    mtx._mtx   =_mtx    ; _mtx   =t_mtx ;
//    index    t_capRow=mtx._capRow;   mtx._capRow=_capRow ; _capRow=t_capRow ;
//    index    t_numRow=mtx._numRow;   mtx._numRow=_numRow ; _numRow=t_numRow ;
//
//    index    t_capCol=mtx._capCol;   mtx._capCol=_capCol ; _capCol=t_capCol ;
//    index    t_numCol=mtx._numCol;   mtx._numCol=_numCol ; _numCol=t_numCol ;    
//}
//CMatrix_f::Num &CMatrix_f::expand(index row, index col    )        //            Variante de acceso a elemento (row,col) que amplia zona en uso e incluso capacidad si necesario
// {    if(row<totalRow() && col<totalCol())    // No need to expand !!!
//        return operator()(row,col);                                            
//    if(col<totalCol())                        // No need to expand columns, only expand Rows!!! (row >= totalRow() )==true
//    {    Expand2RowsCap    ( row+1 );    
//        _numRow = _capRow ;
//        return operator()(row,col);
//    }
//    index t_capRow = row<_capRow ? _capRow : row+1 ;
//    CMatrix_f t_m(t_capRow,col+1);
//    t_m.Copy(*this );
//    swap(t_m) ;
//    return this->operator()(row,col); 
// }
//bool CMatrix_f::Resize            ()            // decidir si vale la pena "optimizar", creciendo solo si es necesario, solo row, o solo col
//{    if ( _numRow < _capRow      ||     _numCol < _capCol )    
//    {        forceResize (_numRow,_numCol);
//            return true ;
//    } else    return false;
//}
//bool CMatrix_f::Reset            (index NumRow, index NumColumns) // error
//{    _numRow=NumRow;    _numCol=NumColumns ;
//    return Resize();
//}
//void CMatrix_f::Copy            (const CMatrix_f& mtx    )    //    copia contenido, aumentando zona en uso y cap si necesario. Conserva datos propios no reescritos - Copy
//{    if  (  _capRow >= mtx.totalRow()  &&  _capCol >= mtx.totalCol() )
//    {    for        ( index r=0        ; r<totalRow()    ; ++r) 
//        for        ( index c=0        ; c<totalCol()    ; ++c) this->operator()(r,c)= mtx(r,c) ;
//        _numRow =  _numRow < mtx.totalRow() ? mtx.totalRow() :  _numRow ;
//        _numCol =  _numCol < mtx.totalCol() ? mtx.totalCol() :  _numCol ;
//    } else 
//        if (_capCol >= mtx.totalCol() )
//        {    Expand2RowsCap( mtx.totalCol()    ) ;
//            Copy (mtx);
//        } else 
//            {    forceResize(mtx.totalRow(), mtx.totalCol() ); 
//                Copy(mtx);}
//}
//

//        template <typename Num>
//bool CMatrix_RA<Num>::Use            (int NumRow, int NumColumns)
//{    _numRow=NumRow;    _numCol=NumColumns ;
//    return Resize();
//}





//        template <typename Num>
//CMatrix_CRA<Num>::CMatrix_CRA(int CapRow, int CapColumns) : _capRow(0), _numRow(CapRow), _capCol(0), _numCol(CapColumns)
//{    
//    forceResize ();    
//}
//        template <typename Num>
//void CMatrix_CRA<Num>::Delete()
//{    while (_capRow) 
//        delete _mtx[--_capRow] ; 
//    delete []_mtx ; 
//    _mtx=nullptr; 
//    _capCol=0; 
//}
//
//        template <typename Num>
//void CMatrix_CRA<Num>::forceResize (int CapRow, int CapColumns)
//{    Delete();
//
//    try            { _mtx=new CRow[CapRow];  _capRow = CapRow ;}
//    catch (...)    { Delete();    /*??*/        throw;             }     // para el futuro de CExeption
//
//    for (int r=0; r<_capRow ; ++r)    _mtx[r]= nullptr;
//
//    try            { for (int r=0; r<_capRow ; ++r)    _mtx[r]=new Num[CapColumns];     _capCol = CapColumns ;}
//    catch (...)    { Delete();    /*??*/        throw; } 
//}
//        template <typename Num>
//void CMatrix_CRA<Num>::forceResize  (){        forceResize (_numRow,_numCol);}
//
//        template <typename Num>
//bool CMatrix_CRA<Num>::Resize        ()
//{    if ( _numRow < _capRow      ||     _numCol < _capCol )    
//    {        forceResize (_numRow,_numCol);
//            return true ;
//    } else    return false;
//}
//        template <typename Num>
//bool CMatrix_CRA<Num>::Use            (int NumRow, int NumColumns)
//{    _numRow=NumRow;    _numCol=NumColumns ;
//    return Resize();
//}
#endif
