//----------------------------------------------------------------------------
/// @file range.hpp
/// @brief
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_GENERAL_UTIL_RANGE_HPP
#define __BOOST_SORT_GENERAL_UTIL_RANGE_HPP
#include <functional>
#include <memory>
#include <type_traits>
#include <vector>
#include <boost/sort/parallel/util/util_iterator.hpp>
#include <boost/sort/parallel/util/algorithm.hpp>
#include <boost/sort/parallel/util/buffer.hpp>

namespace boost
{
namespace sort
{
namespace parallel
{
namespace util
{
///---------------------------------------------------------------------------
/// @struct range
/// @brief this represent a range between two iterators
/// @tparam iter_t type of paramenters of the range
/// @remarks
//----------------------------------------------------------------------------
template <class iter_t>
struct range
{   //------------------------ variables -------------------
    iter_t first , last ;

    //------------------------- functions ------------------
    range ( void){};
    range ( iter_t frs, iter_t lst):first ( frs),last (lst){};

    bool empty ( void )const{ return (first == last );};
    bool not_empty ( void )const{ return (first != last );};
    size_t size( void) const { return (last-first);}
};
//
//-----------------------------------------------------------------------------
//  function : uninit_move
/// @brief Move the objets of a range to uninitialized memory
/// @tparam iter_t : iterator to the elements
/// @tparam value_t : class of the object to construct
/// @param [in] Ptr : pointer to the memory where to create the object
/// @param [in] R : range to move
//-----------------------------------------------------------------------------
template <class iter_t ,
          class value_t = typename iter_value<iter_t>::type >
inline value_t * uninit_move ( value_t *Ptr, const range<iter_t> &R)
{   //---------------------------- begin -------------------------------------
    return uninit_move_buf ( Ptr, R.first, R.last);
};
//
//-----------------------------------------------------------------------------
//  function : destroy
/// @brief Destroy the objects of a range, obtaining uninitialized memory
/// @tparam iter_t : iterator to the elements
/// @param [in] R : range to move
//-----------------------------------------------------------------------------
template <class iter_t >
inline void destroy ( const range<iter_t> &R) { destroy ( R.first, R.last ); };
//
//-----------------------------------------------------------------------------
//  function : move
/// @brief Move the objets of a range to the place pointed by it_dest
/// @tparam iter1_t : type of the iterators of the range
/// @tparam iter2_t : type of the iterators where move the objects
/// @param [in] it_dest : iterator to the final place of the objects
/// @param [in] R : range to move
//-----------------------------------------------------------------------------
template <class iter1_t , class iter2_t >
inline iter2_t move ( iter2_t it_dest, const range<iter1_t> &R)
{   //-------------------------- begin -----------------------------------
    return ( move_buf (it_dest, R.first, R.last));
};
//
//-----------------------------------------------------------------------------
//  function : full_merge
/// @brief Merge two ranges of iter1_t iterators, in the memory pointed by
///        the it_out itertor
/// @tparam iter1_t : iterator to the input buffers
/// @tparam iter2_t : iterator to the output buffers
/// @tparam compare : object to compate the elements pointed by iter1_t
/// @param [in] it_out : iterator where copy the merge of the two ranges
/// @param [in] R1 : first range to merge
/// @param [in] R2 : second range to merge
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template <class iter1_t, class iter2_t, class compare >
inline iter2_t full_merge ( iter2_t it_out, const range<iter1_t> &R1,
                            const range<iter1_t> &R2 , compare  comp)
{   //----------------- checking types -------------------------------
    typedef typename iter_value<iter1_t>::type value1_t ;
    typedef typename iter_value<iter2_t>::type value2_t ;
    static_assert ( std::is_same<value1_t, value2_t>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    return full_merge( R1.first, R1.last,R2.first, R2.last, it_out,comp);
};
//-----------------------------------------------------------------------------
//  function : uninit_full_merge
/// @brief Merge two ranges, and put the result in uninitialized memory pointed
///        by it_out
/// @tparam iter_t : iterator to the input ranges
/// @tparam value_t : type of the objects pointed by iter_t,
/// @tparam compare : object to compate the elements pointed by iter1_t
/// @param [in] it_out : pointer to the memory where create the objects from
///                      the merge
/// @param [in] R1 : first range
/// @param [in] R2 : second range
/// @param [in] comp : comparison object of two value_t elements
//-----------------------------------------------------------------------------
template <class iter_t, class value_t, class compare >
inline value_t* uninit_full_merge ( value_t* it_out,
                                    const range<iter_t> &R1,
                                    const range<iter_t> &R2 ,
                                    compare  comp  )
{   //----------------------- checking types -------------------------------
    typedef typename iter_value<iter_t>::type value1_t ;
    static_assert ( std::is_same<value1_t, value_t>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    return uninit_full_merge (R1.first,R1.last,
                              R2.first,R2.last, it_out,comp);
};
//---------------------------------------------------------------------------
//  function : half_merge
/// @brief : Merge two ranges. The first range is in a separate memory
/// @tparam iter1_t : iterator to the second range, and the destination
/// @tparam iter2_t : iterator to the first range
/// @tparam compare : object to compate the elements pointed by the iterators
/// @param [in] it_dest : iterator where move the merge
/// @param [in] R1 : firt range to merge
/// @param [in] R2 : second range to merge
/// @param [in] comp : object for to make the comparison
//---------------------------------------------------------------------------
template <class iter1_t, class iter2_t, class compare >
iter1_t half_merge ( iter1_t it_dest, range<iter2_t>  R1,
                     range<iter1_t> R2,  compare comp )
{   //------------------------------- begin ------------------------------------
    typedef typename iter_value<iter1_t>::type value1_t ;
    typedef typename iter_value<iter2_t>::type value2_t ;
    static_assert ( std::is_same<value1_t, value2_t>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    //if (not comp (*R2.first, *(R1.last-1))) return move (move(it_dest,R1),R2);
    //if (not comp (*R1.first, *(R2.last-1))) return move (move(it_dest,R2),R1);

    while ( R1.not_empty() and R2.not_empty() )
    {   *(it_dest++)=(not comp(*R2.first,*R1.first))?std::move(*(R1.first++))
                                                    :std::move(*(R2.first++));
    };
    return ( R2.empty())? move (it_dest, R1):it_dest ;
};
//
//############################################################################
//                                                                          ##
//                       F U S I O N     O F                                ##
//                                                                          ##
//              F O U R     E L E M E N T S    R A N G E                    ##
//                                                                          ##
//############################################################################
//

//-----------------------------------------------------------------------------
//  function : less_range
/// @brief Compare the elements pointed fy it1 and it2, and if they
///        are equals, compare their position
/// @tparam iter_t : iterator to compare
/// @tparam compare : object to compate the elements pointed by iter1_t
/// @param [in] it1 : iterator to the first element
/// @param [in] pos1 : position of the object pointed by it1
/// @param [in] it2 : iterator to the second element
/// @param [in] pos2 : position of the element ointed by it2
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template < class iter_t,
           class compare = std::less< typename iter_value<iter_t>::type> >
inline bool less_range ( iter_t it1, uint32_t pos1 ,
                         iter_t it2, uint32_t pos2 ,compare comp =compare() )
{   //----------------------------- begin ------------------------------------
    if ( comp (*it1, *it2 )) return true ;
    if ( pos2 < pos1) return false ;
    return not ( comp (*it2, *it1 )) ;
};

//-----------------------------------------------------------------------------
//  function : full_merge4
/// @brief Merge four ranges
/// @tparam iter1_t : iterator to the output
/// @tparam iter2_t : iterator to the input ranges
/// @tparam compare : object to compate the elements pointed by iter1_t
/// @param [in] it_dest : iterator where move the elements merged
/// @param [in] R : array of ranges to merge
/// @param [in] NR : number of ranges in R
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template <class iter1_t, class iter2_t, class compare >
iter1_t full_merge4 ( iter1_t it_dest, range<iter2_t> R[4],
                          uint32_t NR ,compare comp )
{   //----------------------------- begin ------------------------------------
    if ( NR == 0 ) return it_dest ;
    if ( NR == 1 ) return move ( it_dest, R[0]);
    if ( NR == 2 ) return full_merge ( it_dest, R[0],R[1], comp);

    //------------------------------------------------------------------------
    // Initial sort
    //------------------------------------------------------------------------
    uint32_t Pos[4]={0,1,2,3}, NPos =NR  ;

    if (less_range (R[Pos[1]].first, Pos[1], R[Pos[0]].first,Pos[0],comp))
        std::swap (Pos[0], Pos[1]);
    if (less_range (R[Pos[2]].first, Pos[2], R[Pos[1]].first,Pos[1],comp))
        std::swap (Pos[1], Pos[2]);
    if ( NPos == 4 and
        less_range (R[Pos[3]].first, Pos[3],R[Pos[2]].first, Pos[2], comp) )
        std::swap ( Pos[3], Pos[2]);

    if (less_range (R[Pos[1]].first , Pos[1],R[Pos[0]].first, Pos[0], comp))
        std::swap (Pos[0], Pos[1]);
    if (NPos==4 and
        less_range (R[Pos[2]].first, Pos[2], R[Pos[1]].first,Pos[1],comp))
        std::swap(Pos[1],Pos[2]);

    if (NPos == 4 and
        less_range (R [Pos[1]].first, Pos[1],R[Pos[0]].first, Pos[0], comp))
        std::swap ( Pos[0], Pos[1]);

    while ( NPos > 2)
    {   *(it_dest++) = std::move ( *(R[Pos[0]].first++));
        if (R[Pos[0]].size() == 0   )
        {   Pos[0] = Pos[1];
            Pos[1] = Pos[2];
            Pos[2] = Pos[3];
            --NPos ;
        }
        else
        {   if (less_range(R[Pos[1]].first,Pos[1],R[Pos[0]].first,Pos[0],comp))
            {   std::swap ( Pos[0], Pos[1]);
                if (less_range ( R[Pos[2]].first, Pos[2],
                                 R[Pos[1]].first,Pos[1], comp ))
                {   std::swap ( Pos[1], Pos[2]);
                    if (NPos == 4 and
                        less_range (R[Pos[3]].first,Pos[3],
                                    R[Pos[2]].first, Pos[2], comp))
                    {   std::swap ( Pos[2], Pos[3]);
                    };
                };
            };
        };
    };
    if ( Pos[0]< Pos[1])
        return full_merge ( it_dest, R[Pos[0]],R[Pos[1]], comp);
    else
        return full_merge ( it_dest, R[Pos[1]],R[Pos[0]], comp);
};
//
//-----------------------------------------------------------------------------
//  function : uninit_full_merge4
/// @brief Merge four ranges and put the result in uninitialized memory
/// @tparam iter_t : iterator to the input ranges
/// @tparam value_t : type of object pointed by iter_t
/// @tparam compare : object to compate the elements pointed by iter_t
/// @param [in] it_dest : iterator where move the elements merged
/// @param [in] R : array of ranges to merge
/// @param [in] NR : number of ranges in R
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template <class iter_t, class value_t, class compare >
value_t* uninit_full_merge4 ( value_t* it_dest,  range<iter_t> R[4],
                                         uint32_t NR ,compare comp )
{   //----------------------------- begin ------------------------------------
    typedef typename iter_value<iter_t>::type value1_t ;
    static_assert ( std::is_same<value1_t, value_t>::value,
                   "Incompatible iterators\n");

    if ( NR == 0 ) return it_dest ;
    if ( NR == 1 ) return uninit_move ( it_dest, R[0]);
    if ( NR == 2 ) return uninit_full_merge ( it_dest, R[0],R[1], comp);

    //------------------------------------------------------------------------
    // Initial sort
    //------------------------------------------------------------------------
    uint32_t Pos[4]={0,1,2,3}, NPos =NR  ;

    if (less_range (R[Pos[1]].first, Pos[1],R[Pos[0]].first,Pos[0],comp))
        std::swap (Pos[0], Pos[1]);
    if (less_range (R[Pos[2]].first,Pos[2],R[Pos[1]].first,Pos[1],comp))
        std::swap (Pos[1], Pos[2]);
    if (NPos == 4 and
        less_range (R[Pos[3]].first, Pos[3],R[Pos[2]].first,Pos[2],comp))
        std::swap ( Pos[3], Pos[2]);

    if (less_range (R[Pos[1]].first,Pos[1],R[Pos[0]].first,Pos[0],comp))
        std::swap (Pos[0], Pos[1]);
    if (NPos==4 and
        less_range (R[Pos[2]].first, Pos[2], R[Pos[1]].first,Pos[1],comp))
        std::swap(Pos[1],Pos[2]);

    if (NPos == 4 and
        less_range (R [Pos[1]].first, Pos[1],R[Pos[0]].first, Pos[0], comp))
        std::swap ( Pos[0], Pos[1]);

    while ( NPos > 2)
    {   construct ( (it_dest++),std::move ( *(R[Pos[0]].first++)) );
        if (R[Pos[0]].size() == 0   )
        {   Pos[0] = Pos[1];
            Pos[1] = Pos[2];
            Pos[2] = Pos[3];
            --NPos ;
        }
        else
        {   if ( less_range (R[Pos[1]].first, Pos[1],
                             R[Pos[0]].first,Pos[0],comp))
            {   std::swap ( Pos[0], Pos[1]);
                if ( less_range (R[Pos[2]].first, Pos[2],
                                 R[Pos[1]].first, Pos[1], comp ))
                {   std::swap ( Pos[1], Pos[2]);
                    if (NPos == 4 and less_range (R[Pos[3]].first, Pos[3],
                                                  R[Pos[2]].first,Pos[2], comp))
                    {   std::swap ( Pos[2], Pos[3]);
                    };
                };
            };
        };
    };
    if ( Pos[0] < Pos[1])
        return uninit_full_merge ( it_dest, R[Pos[0]],R[Pos[1]], comp);
    else
        return uninit_full_merge ( it_dest, R[Pos[1]],R[Pos[0]], comp);
};

//-----------------------------------------------------------------------------
//  function : merge_level4
/// @brief merge the ranges in Vin using full_merge4, and the ranges obtained
//         are in Vout.The output ranges are obtained moving the object to the
///        position pointed by it_dest
/// @tparam iter1_t : iterator to the output
/// @tparam iter2_t : iterator to the input ranges
/// @tparam compare : object to compate the elements pointed by iter1_t
/// @param [in] it_dest : iterator where move the elements merged
/// @param [in] Vin : vector of ranges to merge
/// @param [in] Vout : vector of ranges obtained
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template <class iter1_t, class iter2_t, class compare >
iter1_t merge_level4 ( iter1_t it_dest,
                       std::vector<range<iter2_t> > &Vin,
                       std::vector<range<iter1_t> >& Vout ,
                       compare comp )
{   //------------------------------- begin ----------------------------------
    Vout.clear() ;
    if ( Vin.size() == 0) return it_dest;
    if ( Vin.size() == 1 )
    {   iter1_t it_aux = move ( it_dest, Vin[0]);
        Vout.emplace_back ( it_dest, it_aux);
        return it_aux ;
    };

    iter1_t it_out = it_dest ;
    uint32_t Nrange = Vin.size() ;
    uint32_t PosIni = 0;
    while ( PosIni < Vin.size())
    {   uint32_t Nmerge =(Nrange + 3) >> 2 ;
        uint32_t Nelem =  ( Nrange + Nmerge -1) / Nmerge ;
        iter1_t it_aux = full_merge4 ( it_out, & Vin[PosIni], Nelem , comp);
        Vout.emplace_back ( it_out, it_aux );
        it_out = it_aux ;
        PosIni += Nelem ;
        Nrange -= Nelem ;
    };
    return it_out ;
};

//-----------------------------------------------------------------------------
//  function : uninit_merge_level4
/// @brief merge the ranges in Vin using full_merge4, and the ranges obtained
//         are in Vout. The output ranges are obtained creating objects in the
///        uninitialized memory pointed by it_dest.
/// @tparam iter_t : iterator to the input ranges
/// @tparam value_t : objects pointed by iter_t
/// @tparam compare : object to compate the elements pointed by iter1_t
/// @param [in] it_dest : iterator where move the elements merged
/// @param [in] Vin : vector of ranges to merge
/// @param [in] Vout : vector of ranges obtained
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template <class iter_t, class value_t, class compare >
value_t* uninit_merge_level4 (  value_t* it_dest,
                                std::vector<range<iter_t> > &Vin,
                                std::vector<range<value_t*> >& Vout ,
                                compare comp  )
{   //------------------------------- begin ----------------------------------
    Vout.clear() ;
    if ( Vin.size() == 0) return it_dest;
    if ( Vin.size() == 1 )
    {   value_t * paux = uninit_move ( it_dest, Vin[0]);
        Vout.emplace_back ( it_dest,paux);
        return paux ;
    };
    value_t * it_out = it_dest ;
    uint32_t Nrange = Vin.size() ;
    uint32_t PosIni = 0;

    while ( PosIni < Vin.size())
    {   uint32_t Nmerge =(Nrange + 3) >> 2 ;
        uint32_t Nelem =  ( Nrange + Nmerge -1) / Nmerge ;
        value_t *it_aux = uninit_full_merge4(it_out, &Vin[PosIni], Nelem, comp);
        Vout.emplace_back ( it_out, it_aux );
        it_out = it_aux ;
        PosIni += Nelem ;
        Nrange -= Nelem ;
    };
    return it_out ;
};
//
//-----------------------------------------------------------------------------
//  function : merge_vector4
/// @brief This function merge the ranges in Vin and put the final range in Vout
/// @tparam iter1_t : iterator to the input buffers
/// @tparam iter2_t : iterator to the output buffers
/// @tparam compare : object to compate the elements pointed by iter1_t
/// @param [in] first1 iterator to the first position of the Vin ranges
/// @param [in] first2 iterator to the first position of the Vout ranges
/// @param [in] Vin : vector of input ranges
/// @param [in] Vout : vector of output ranges
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template <class iter1_t , class iter2_t , class compare >
void merge_vector4 ( iter1_t first1, iter2_t first2,
                    std::vector<range<iter1_t> > &Vin,
                    std::vector<range<iter2_t> > &Vout, compare comp)
{   //---------------------------------- begin -------------------------------
    typedef typename iter_value<iter1_t>::type value1_t ;
    typedef typename iter_value<iter2_t>::type value2_t ;
    static_assert ( std::is_same<value1_t, value2_t>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    Vout.clear() ;
    bool SW = false ;
    uint32_t Nrange = Vin.size() ;
    if ( Nrange == 0) return ;

    while ( not SW or Nrange >1)
    {   if ( Nrange == 1)
        {   iter2_t it_aux = move ( first2, Vin[0]);
        	Vout.emplace_back ( first2, it_aux);
            return;
        };
        if ( SW )
        {   merge_level4 ( first1, Vout, Vin, comp );
            SW = false ;
            Nrange = Vin.size() ;
        }
        else
        {   merge_level4 ( first2 , Vin, Vout, comp);
            SW = true ;
            Nrange = Vout.size() ;
        };
    };
};
//****************************************************************************
};//    End namespace util
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
