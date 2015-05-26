//----------------------------------------------------------------------------
/// @file merge_sort.hpp
/// @brief Merge Sort algorithm
///
/// @author Copyright (c) 2010 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_GENERAL_ALGORITHM_MERGE_SORT_HPP
#define __BOOST_SORT_GENERAL_ALGORITHM_MERGE_SORT_HPP

#include <functional>
#include <memory>
#include <type_traits>
#include <vector>
#include <boost/sort/parallel/util/util_iterator.hpp>
#include <boost/sort/parallel/util/buffer.hpp>
#include <boost/sort/parallel/algorithm/insertion_sort.hpp>
#include <boost/sort/parallel/algorithm/token.hpp>
#include <boost/sort/parallel/algorithm/indirect.hpp>



namespace boost
{
namespace sort
{
namespace parallel
{
namespace algorithm
{
using namespace boost::sort::parallel::util ;

template < class iter_t,
           typename compare=std::less<typename iter_value<iter_t>::type>  >
struct merge_sort_tag
{
//----------------------------------------------------------------------------
//              DEFINITIONS AND CONSTANTS
//----------------------------------------------------------------------------
typedef typename iter_value<iter_t>::type           value_t ;
static const uint32_t MSort_min = 36 ;

//----------------------------------------------------------------------------
//                        VARIABLES
//----------------------------------------------------------------------------
value_t *Ptr ;
bool owner ;
//
//---------------------------------------------------------------------------
//  function : merge_sort
/// @brief Merge Sort algorithm ( stable sort)
/// @param [in] first : first iterator to the elements to sort
/// @param [in] last : last iterator to the elements to sort
/// @param [in] P : pointer of a data area with at least (N+1)/2 elements
/// @exception
/// @return
/// @remarks
//---------------------------------------------------------------------------
merge_sort_tag (iter_t first, iter_t last, compare comp = compare() )
                :Ptr( nullptr),owner ( false)
{   //-------------------------- begin ------------------------------
    assert ((last - first)>= 0 );
    size_t NElem = size_t (last - first) ;
    size_t NPtr = ( NElem + 1) >>1 ;

    if ( NElem <= MSort_min)
    {   insertion_sort ( first, last , comp);
        return  ;
    };

    Ptr = std::get_temporary_buffer<value_t> ( NPtr ).first;
    if ( Ptr == nullptr) throw std::bad_alloc ( );
    owner = true ;
    merge_sort_internal ( first , last , comp,Ptr );
};
//
//---------------------------------------------------------------------------
//  function : merge_sort
/// @brief Merge Sort algorithm ( stable sort)
/// @param [in] first : first iterator to the elements to sort
/// @param [in] last : last iterator to the elements to sort
/// @param [in] P : pointer of a data area with at least (N+1)/2 elements
/// @exception
/// @return
/// @remarks
//---------------------------------------------------------------------------
merge_sort_tag (iter_t first, iter_t last, compare comp ,
                typename iter_value<iter_t>::type * P1, size_t NP1 )
                :Ptr( P1),owner ( false)
{   //-------------------------- begin ------------------------------
    assert ( P1 != nullptr);
    assert ((last - first)>= 0 );
    size_t NElem = size_t (last - first) ;
    size_t NPtr = ( NElem + 1) >>1 ;
    assert ( NP1 >= NPtr);

    if ( NElem <= MSort_min)
    {   insertion_sort ( first, last , comp);
        return  ;
    };
    merge_sort_internal ( first , last , comp,Ptr );
};
~merge_sort_tag ()
{   //-------------------------- begin ---------------------------------------
    if (owner and Ptr != nullptr )std::return_temporary_buffer ( Ptr) ;
};
//
//---------------------------------------------------------------------------
//  function : merge_sort_internal
/// @brief Merge Sort algorithm ( stable sort)
/// @param [in] first : first iterator to the elements to sort
/// @param [in] last : last iterator to the elements to sort
/// @param [in] P : pointer of a data area with at least (N+1)/2 elements
/// @exception
/// @return
/// @remarks
//---------------------------------------------------------------------------
inline void merge_sort_internal ( iter_t first , iter_t last,
                                  compare comp, value_t *P )
{   //---------------------------- begin ------------------------------------
    size_t N = size_t (last - first) ;
    size_t N2 = ( N+1)>>1 ;

    if ( N2 > MSort_min)
    {   merge_sort_internal ( first , first + N2, comp,P );
        merge_sort_internal ( first + N2 , last , comp, P );
    }
    else
    {   insertion_sort ( first , first + N2,comp );
        insertion_sort ( first + N2 , last ,comp );
    };
    //-----------------------------------------------------------------------
    // copy the first N2 elements in P
    //-----------------------------------------------------------------------
    if ( not comp ( *(first + N2) , *(first+(N2-1))))  return ;
    uninit_move_buf ( P, first, first+ N2 );

    //-----------------------------------------------------------------------
    // Fusion
    //-----------------------------------------------------------------------
    iter_t P2 = first + N2 , P2_end= last, Alfa = first ;
    value_t *P1 = P , *P1_end = P + N2 ;
    while ( P2 != P2_end and Alfa != P2)
    {   *(Alfa++)= (comp(*P2,*P1) )?std::move(*(P2++)):std::move(*(P1++));
    }
    if ( P2 == P2_end)   move_buf ( Alfa, P1, P1_end);
    destroy ( P, P1_end);
//----------------------------------------------------------------------------
};//        End function merge_sort_tag
//----------------------------------------------------------------------------

//
//----------------------------------------------------------------------------
};//        End of class merge_sort_tag
//----------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//  function : merge_sort
/// @brief Merge Sort algorithm ( stable sort)
/// @param [in] first : first iterator to the elements to sort
/// @param [in] last : last iterator to the elements to sort
/// @param [in] comp : object for to compare two elements
/// @exception
/// @return
/// @remarks
//---------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less<typename iter_value<iter_t>::type>
         >
void merge_sort (iter_t first, iter_t last, compare comp = compare() )
{   //----------------------------- begin ----------------------------
    merge_sort_tag<iter_t,compare> ( first, last , comp);
};
//
//---------------------------------------------------------------------------
//  function : merge_sort
/// @brief Merge Sort algorithm ( stable sort)
/// @param [in] first : first iterator to the elements to sort
/// @param [in] last : last iterator to the elements to sort
/// @param [in] comp : object for to compare two elements
/// @param [in] P1 : pointer of a data area with at least (N+1)/2 elements
/// @param [in] NP1 : capacity of the memory area pointed by P1
/// @exception
/// @return
/// @remarks
//---------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less<typename iter_value<iter_t>::type>
         >
void merge_sort (iter_t first, iter_t last, compare comp ,
                typename iter_value<iter_t>::type * P1, size_t NP1  )
{   //----------------------------- begin ----------------------------
    merge_sort_tag<iter_t,compare> ( first, last , comp,P1,NP1);
};
//############################################################################
//                                                                          ##
//                I N D I R E C T     F U N C T I O N S                     ##
//                                                                          ##
//############################################################################
//
//---------------------------------------------------------------------------
//  function : indirect_merge_sort
/// @brief Merge Sort algorithm using an indirect sorting( stable sort)
/// @param [in] first : first iterator to the elements to sort
/// @param [in] last : last iterator to the elements to sort
/// @param [in] comp : object for to compare two elements
/// @exception
/// @return
/// @remarks
//---------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less<typename iter_value<iter_t>::type>
         >
void indirect_merge_sort ( iter_t first, iter_t last, compare comp = compare())
{   //------------------------------- begin--------------------------
    typedef less_ptr_no_null <iter_t, compare>      compare_ptr ;

    std::vector<iter_t> VP ;
    create_index ( first , last , VP);
    merge_sort  ( VP.begin() , VP.end(), compare_ptr(comp) );
    sort_index ( first , VP) ;
};
//
//****************************************************************************
};//    End namespace algorithm
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
