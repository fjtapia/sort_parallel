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
#ifndef __BOOST_SORT_PARALLEL_DETAIL_INSERTION_SORT_HPP
#define __BOOST_SORT_PARALLEL_DETAIL_INSERTION_SORT_HPP

#include <iterator>
#include <functional>
#include <utility> // std::swap

namespace boost		{
namespace sort		{
namespace parallel	{
namespace detail	{


//-----------------------------------------------------------------------------
//  function : insertion_sort
/// @brief : Insertion sort algorithm
/// @param [in] first: iterator to the first element of the range
/// @param [in] last : iterator to the next element of the last in the range
/// @param [in] comp : object for to do the comparison between the elements
/// @remarks This algorithm is O(N²)
//-----------------------------------------------------------------------------
template < class Iter_t, typename Compare >
void insertion_sort (Iter_t first, Iter_t last, Compare comp )
{   //------------------------- begin ----------------------
    typedef typename std::iterator_traits<Iter_t>::value_type value_t ;

    if ( (last-first) < 2 ) return ;
    for (Iter_t curr = first + 1; curr != last; ++curr)
    {   
        value_t curr_value = std::move (*curr);
        Iter_t target_location = curr;

        while(target_location != first and comp(curr_value, *(target_location - 1)))
        {   *target_location = std::move(*(target_location - 1));
            --target_location;
        };
        *target_location = std::move (curr_value);
    };
};
//
//****************************************************************************
};//    End namespace detail
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
