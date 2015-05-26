//----------------------------------------------------------------------------
/// @file intro_sort.hpp
/// @brief Intro Sort algorithm
///
/// @author Copyright (c) 2010 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_GENERAL_ALGORITHM_INTRO_SORT_HPP
#define __BOOST_SORT_GENERAL_ALGORITHM_INTRO_SORT_HPP

#include <boost/sort/parallel/util/algorithm.hpp>
#include <boost/sort/parallel/algorithm/insertion_sort.hpp>
#include <boost/sort/parallel/algorithm/heap_sort.hpp>
#include <boost/sort/parallel/algorithm/indirect.hpp>
#include <vector>
#include <type_traits>

namespace boost
{
namespace sort
{
namespace parallel
{
namespace algorithm
{

using boost::sort::parallel::util::MS1B;


template < class iter_t,
           typename compare = std::less < typename iter_value<iter_t>::type >
         >
bool check_if_sort( iter_t first , iter_t last,compare comp );
//
//-----------------------------------------------------------------------------
//  function : intro_sort_internal
/// @brief : internal function for to divide and sort the ranges
/// @tparam iter_t : iterator to the elements
/// @tparam compare : object for to compare two elements pointed by iter_t
///                   iterators
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the element after the last in the range
/// @param [in] Level : Level of deep from the initial range
/// @param [in] comp : object for to compare elements
/// @exception
/// @return
/// @remarks This function don't be called directly by the users
//-----------------------------------------------------------------------------
template< class iter_t,
          typename compare = std::less < typename iter_value<iter_t>::type >
        >
inline void intro_sort_internal( iter_t first , iter_t last,
                                  uint32_t Level ,compare comp = compare())
{   //------------------------------ begin -----------------------------------
    typedef typename iter_value<iter_t>::type value_t ;

    //-------------------------- sort process --------------------------------
    const uint32_t NMin = 32 ;
    auto N = last - first;

    if ( N < NMin)   return insertion_sort(first,last,comp);
    if (  Level == 0)   return heap_sort ( first , last,comp);


    //------------------- check if sort --------------------------------------
    bool SW = true ;
    for ( iter_t it1 = first, it2 = first+1 ;
          it2 != last and (SW = not comp(*it2,*it1));it1 = it2++);
    if (SW) return ;

    //----------------------------------------------------------------
    //                     split
    //----------------------------------------------------------------
    size_t N2 = ( size_t (N +1 ) >>1 ) ;

    std::swap ( *first , * (first + N2 ));
    value_t &  val = const_cast < value_t &>(*first);

    iter_t c_first = first+1 , c_last  = last-1;

    while ( c_first != last and comp (*c_first, val    )) ++c_first ;
    if ( c_first == last)
    {   std::swap ( *first , * (last-1));
        intro_sort_internal ( first , last-1, Level -1 , comp);

    }
    else
    {
        while ( comp (val     ,*c_last )) --c_last ;
        while (not( c_first > c_last ))
        {   std::swap ( *(c_first++), *(c_last--));
            while (comp (*c_first , val) ) ++c_first;
            while (comp( val, *c_last ) )  --c_last ;
        }; // End while
        std::swap ( *first , * c_last);

        if ((c_last - first ) > 1 )intro_sort_internal (first , c_last , Level -1, comp);
        if ((last - c_first ) > 1 )intro_sort_internal (c_first, last, Level -1 , comp);
    };

} ;

//
//-----------------------------------------------------------------------------
//  function : intro_sort
/// @brief : function for to sort range [first, last )
/// @tparam iter_t : iterator to the elements
/// @tparam compare : object for to compare two elements pointed by iter_t
///                   iterators
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the element after the last in the range
/// @param [in] comp : object for to compare elements
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less <typename iter_value<iter_t>::type>
         >
void intro_sort ( iter_t first, iter_t last,compare comp = compare())
{   //------------------------- begin ----------------------
    auto N = last - first;
    assert ( N > 0);

    uint32_t Level = (MS1B(N) *3);
    intro_sort_internal ( first , last, Level,comp);
};
//
//-----------------------------------------------------------------------------
//  function : sort
/// @brief : function for to sort range [first, last )
/// @tparam iter_t : iterator to the elements
/// @tparam compare : object for to compare two elements pointed by iter_t
///                   iterators
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the element after the last in the range
/// @param [in] comp : object for to compare elements
/// @exception
/// @return
/// @remarks this function is an alias of intro_sort
//-----------------------------------------------------------------------------
template< class iter_t,
          typename compare = std::less < typename iter_value<iter_t>::type > >
inline void sort( iter_t first , iter_t last,
                        uint32_t Level ,compare comp = compare())
{   //------------------------- begin ----------------------
    intro_sort ( first, last, comp);
};

//############################################################################
//                                                                          ##
//                I N D I R E C T     F U N C T I O N S                     ##
//                                                                          ##
//############################################################################
//
//-----------------------------------------------------------------------------
//  function : indirect_intro_sort
/// @brief : function for to implement an indirect sort range [first, last )
/// @tparam iter_t : iterator to the elements
/// @tparam compare : object for to compare two elements pointed by iter_t
///                   iterators
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the element after the last in the range
/// @param [in] comp : object for to compare elements
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less<typename iter_value<iter_t>::type>
         >
void indirect_intro_sort ( iter_t first, iter_t last ,
                                    compare comp = compare() )
{   //------------------------------- begin--------------------------
    typedef less_ptr_no_null <iter_t, compare>      compare_ptr ;

    std::vector<iter_t> VP ;
    create_index ( first , last , VP);
    intro_sort  ( VP.begin() , VP.end(), compare_ptr(comp) );
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
