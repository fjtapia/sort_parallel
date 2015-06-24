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
#ifndef __BOOST_SORT_ALGORITHM_INTRO_SORT_HPP
#define __BOOST_SORT_ALGORITHM_INTRO_SORT_HPP

#include <boost/sort/util/algorithm.hpp>
#include <boost/sort/algorithm/insertion_sort.hpp>
#include <boost/sort/algorithm/heap_sort.hpp>
#include <boost/sort/algorithm/indirect.hpp>
#include <vector>
#include <type_traits>

namespace boost
{
namespace sort
{
namespace algorithm
{

using boost::sort::util::MS1B;
//-----------------------------------------------------------------------------
//  function :
/// @brief
/// @param [in/out]
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template< class iter_t,
          typename compare = std::less < typename iter_value<iter_t>::type >
        >
inline void intro_sort_internal( iter_t first , iter_t last,
                                  uint32_t Level ,compare comp = compare())
{   //------------------------- begin ----------------------
    const uint32_t NMin = 32 ;
    auto N = last - first;
    if ( N < NMin)   return insertion_sort(first,last,comp);
    if ( Level == 0) return   heap_sort ( first , last,comp);

    //----------------------------------------------------------------
    //                     split
    //----------------------------------------------------------------
    typedef typename std::remove_reference<decltype(*first)>::type value_t ;

    value_t val = const_cast < value_t &>(*(first +(N>>1)) );

    iter_t c_first = first , c_last  = last-1;
    while ( comp (*c_first, val    )) ++c_first ;
    while ( comp (val     ,*c_last )) --c_last ;
    while (not( c_first > c_last ))
    {   std::swap ( *(c_first++), *(c_last--));
        while (comp (*c_first , val) ) ++c_first;
        while (comp( val, *c_last ) )  --c_last ;
    }; // End while

    intro_sort_internal (first , c_last +1, Level -1, comp);
    intro_sort_internal (c_first, last, Level -1 , comp);
} ;


template < class iter_t,
           typename compare = std::less <typename iter_value<iter_t>::type>
         >
void intro_sort ( iter_t first, iter_t last,compare comp = compare())
{   //------------------------- begin ----------------------
    auto N = last - first;
    assert ( N > 0);
    uint32_t Level = (MS1B ( N)+1)<<1 ;
    intro_sort_internal ( first , last, Level,comp);
};
//-----------------------------------------------------------------------------
//  function :
/// @brief
/// @param [in/out]
/// @exception
/// @return
/// @remarks
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
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
