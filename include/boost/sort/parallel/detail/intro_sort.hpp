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
#ifndef __BOOST_SORT_PARALLEL_ALGORITHM_INTRO_SORT_HPP
#define __BOOST_SORT_PARALLEL_ALGORITHM_INTRO_SORT_HPP

#include <algorithm>
#include <vector>
#include <type_traits>
#include <iterator>
#include <boost/sort/parallel/detail/util/nbits.hpp>
#include <boost/sort/parallel/detail/insertion_sort.hpp>
#include <boost/sort/parallel/detail/heap_sort.hpp>
#include <boost/sort/parallel/detail/indirect.hpp>
#include <boost/sort/parallel/detail/util/compare_traits.hpp>

namespace boost		{
namespace sort		{
namespace parallel 	{
namespace detail	{

namespace bsp_util = boost::sort::parallel::detail::util ;
using bsp_util::compare_iter ;
using bsp_util::nbits64;

template< typename Iter_t, typename Compare>
inline Iter_t mid3(Iter_t it_l, Iter_t it_m, Iter_t it_r,Compare comp)
{	return comp(* it_l, *it_m)
		?( comp(*it_m, *it_r) ? it_m : ( comp(*it_l, *it_r) ? it_r:it_l))
        :( comp(*it_r, *it_m) ? it_m : ( comp(*it_r, *it_l) ? it_r:it_l));
};

//----------------------------------------------------------------------------
// calculate the pivoting using a mid of 3 and move to the firat position
//----------------------------------------------------------------------------
template <class Iter_t , class Compare >
inline void pivot3 ( Iter_t first, Iter_t last, Compare comp)
{   auto N2 = ( last - first ) >>1 ;
	Iter_t it_val = mid3 ( first +1, first + N2, last-1,comp);
    std::swap ( *first , * it_val);
};

template <class Iter_t , class Compare >
inline Iter_t mid9 ( Iter_t it1, Iter_t it2 , Iter_t it3 ,
		             Iter_t it4 , Iter_t it5, Iter_t it6,
					 Iter_t it7, Iter_t it8, Iter_t it9, Compare comp)
{	//-------------------------- begin ---------------------------------------
	return mid3 (mid3(it1, it2, it3, comp ),
		         mid3(it4, it5, it6,comp  ),
				 mid3(it7, it8, it9,comp), comp);
};

template <class Iter_t , class Compare >
inline void pivot9 ( Iter_t first, Iter_t last, Compare comp)
{	//----------------------------- begin ------------------------------------
	size_t cupo = (last - first) >>3 ;
	Iter_t itaux = mid9 (first+1, first+cupo, first+2*cupo,
  	                	first+3*cupo, first + 4*cupo, first + 5*cupo,
						first + 6*cupo, first + 7*cupo,last-1,comp);
	std::swap ( *first , * itaux);
};
//
//-----------------------------------------------------------------------------
//  function : intro_sort_internal
/// @brief : internal function for to divide and sort the ranges
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the element after the last in the range
/// @param [in] level : level of depth from the initial range
/// @param [in] comp : object for to Compare elements
//-----------------------------------------------------------------------------
template< class Iter_t, typename Compare  >
void intro_sort_internal( Iter_t  first , Iter_t  last,
		                  uint32_t level ,Compare comp )
{   //------------------------------ begin -----------------------------------
    typedef typename std::iterator_traits<Iter_t>::value_type  value_t ;

    const uint32_t nmin = 32 ;
    size_t nelem = last - first;
    if ( nelem  < nmin )   return insertion_sort( first , last,comp);
    if ( level == 0)
    {	heap_sort<Iter_t, Compare> ( first , last,comp);
    	return ;
    };

    //--------------------- division ----------------------------------
    pivot3 ( first, last, comp);

    const value_t & val = const_cast < value_t &>(* first);
    Iter_t c_first = first+1 , c_last  = last-1;

    while ( comp(*c_first, val) ) ++c_first ;
    while ( comp ( val,*c_last ) ) --c_last ;
    while (not( c_first > c_last ))
    {   std::swap ( *(c_first++), *(c_last--));
        while ( comp (*c_first, val) ) ++c_first;
        while ( comp ( val, *c_last) ) --c_last ;
    }; // End while
    std::swap ( *first , * c_last);
    intro_sort_internal (first , c_last, level -1, comp);
    intro_sort_internal (c_first, last, level -1 , comp);
};

//
//-----------------------------------------------------------------------------
//  function : intro_sort
/// @brief : function for to sort range [first, last )
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the element after the last in the range
/// @param [in] comp : object for to compare elements
//-----------------------------------------------------------------------------
template < class Iter_t,  typename Compare  >
void intro_sort ( Iter_t first, Iter_t last,Compare comp )
{   //------------------------- begin ----------------------
    auto nelem = last - first;
    assert ( nelem >= 0);
    //------------------- check if sort --------------------------------------
    //if (std::is_sorted ( first, last ,comp)) return ;
    //------------------- check if sort --------------------------------------
    bool sw = true ;
    for ( Iter_t it1 = first, it2 = first+1 ;
          it2 != last and (sw = not comp(*it2,*it1));it1 = it2++);
    if (sw) return ;

    uint32_t level = ((nbits64(nelem)-4) *3)/2;
    intro_sort_internal ( first , last, level,comp);
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
