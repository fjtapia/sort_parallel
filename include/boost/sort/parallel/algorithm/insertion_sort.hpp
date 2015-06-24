//----------------------------------------------------------------------------
/// @file insertion_sort.hpp
/// @brief Insertion Sort algorithm
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_ALGORITHM_INSERTION_SORT_HPP
#define __BOOST_SORT_PARALLEL_ALGORITHM_INSERTION_SORT_HPP

#include <boost/sort/parallel/util/definition.hpp>
#include <boost/sort/parallel/util/util_iterator.hpp>
#include <functional>
#include <stdexcept>
#include <cstdint>
#include <cassert>
#include <utility> // std::swap

namespace boost
{
namespace sort
{
namespace parallel
{
namespace algorithm
{
using  boost::sort::parallel::util::iter_value;
//-----------------------------------------------------------------------------
//  function : insertion_sort
/// @brief : Insertion sort algorithm
/// @param [in] first: iterator to the first element of the range
/// @param [in] last : iterator to the next element of the last in the range
/// @param [in] comp : object for to do the comparison between the elements
/// @remarks This algorithm is O(N²)
//-----------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less < typename iter_value<iter_t>::type >
         >
inline void insertion_sort (iter_t first, iter_t last, compare comp=compare())
{   //------------------------- begin ----------------------
    typedef typename iter_value<iter_t>::type value_t ;

    if ( (last-first) < 2 ) return ;
    for ( iter_t Alfa = first +1 ;Alfa != last ; ++Alfa)
    {   value_t Aux = std::move ( *Alfa);
        iter_t Beta  = Alfa ;

        while( Beta != first and comp ( Aux, *(Beta-1) ) )
        {   *Beta = std::move ( *(Beta-1));
            --Beta ;
        };
        *Beta = std::move ( Aux );
    };
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
