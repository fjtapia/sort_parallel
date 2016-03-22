//----------------------------------------------------------------------------
/// @file sort.hpp
/// @brief This file contains the sort functions of the sort library
///
/// @author Copyright (c) 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_SORT_HPP
#define __BOOST_SORT_PARALLEL_SORT_HPP

#include <iterator>
#include <boost/sort/parallel/algorithm/parallel_stable_sort.hpp>
#include <boost/sort/parallel/algorithm/block_indirect_sort.hpp>
#include <boost/sort/parallel/algorithm/parallel_greedy_sort.hpp>

namespace boost 	{
namespace sort		{
namespace parallel	{
//
//****************************************************************************
//             USING AND DEFINITIONS
//****************************************************************************
namespace bs_algo = boost::sort::parallel::algorithm ;
namespace bs_util = boost::sort::parallel::util;
namespace bs_tools = boost::sort::parallel::tools;

//----------------------------------------------------------------------------
// The code of the class NThread is in boost/sort/parallel/util/atomic.hpp
//----------------------------------------------------------------------------
using std::iterator_traits ;
using bs_tools::NThread ;
using bs_tools::NThread_HW ;
using bs_algo::less_ptr_no_null;

//
//-----------------------------------------------------------------------------
//  function : sort
/// @brief this function implement a non stable sort, based internally in the
///        intro_sort algorithm. Run with 1 thread
/// @tparam iter_t : iterators for to access to the elements
/// @tparam compare : object for to compare two elements pointed by the iter_t
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
//-----------------------------------------------------------------------------
template < class iter_t,
           typename compare
		   = std::less <typename iterator_traits<iter_t>::value_type> >
inline void sort ( iter_t first, iter_t last,compare comp = compare())
{   //---------------------------- begin -------------------------------------
    bs_algo::intro_sort(first, last,comp);
};

template < class iter_t,
           typename compare
		   = std::less <typename iterator_traits<iter_t>::value_type> >
inline void indirect_sort ( iter_t first, iter_t last,
		                         compare comp = compare())
{   //---------------------------- begin -------------------------------------
    bs_algo::indirect_intro_sort(first, last,comp);
};
//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief this function implement a non stable parallel sort. The number of
///        threads to use is defined by the NThread parameter
/// @tparam iter_t : iterators for to access to the elements
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] NT : This object is a integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
//-----------------------------------------------------------------------------
template < class iter_t >
inline void parallel_sort ( iter_t first, iter_t last ,
		                            NThread NT = NThread() )
{   //----------------------------------- begin ------------------------------
    if ( NT() < 9 )
		bs_algo::parallel_greedy_sort( first, last,  NT);
	else
        bs_algo::block_indirect_sort ( first, last, NT);
};
//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief this function implement a non stable parallel sort. The number of
///        threads to use is defined by the NThread parameter
/// @tparam iter_t : iterators for to access to the elements
/// @tparam compare : object for to compare two elements pointed by the iter_t
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
/// @param [in] NT : This object is a integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
//-----------------------------------------------------------------------------
template < class iter_t,
          typename compare
		  = std::less <typename iterator_traits<iter_t>::value_type>  >
inline void parallel_sort ( iter_t first, iter_t last,
                                           compare comp,  NThread NT= NThread())
{   //----------------------------- begin ----------------------------------
    if ( NT() < 9 )
		bs_algo::parallel_greedy_sort( first, last, comp, NT);
	else
        bs_algo::block_indirect_sort ( first, last, comp, NT);
};
//
//-----------------------------------------------------------------------------
//  function : stable_sort
/// @brief this function implement a stable sort, based internally in the new
///        smart_merge_sort algorithm. Run with 1 thread
/// @tparam iter_t : iterators for to access to the elements
/// @tparam compare : object for to compare two elements pointed by the iter_t
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
//-----------------------------------------------------------------------------
template < class iter_t,
           typename compare
		   = std::less<typename iterator_traits<iter_t>::value_type>  >
inline void stable_sort(iter_t first, iter_t last, compare comp = compare() )
{   //--------------------------------- begin --------------------------------
    bs_algo::spin_sort(first, last,comp);
};
template < class iter_t,
           typename compare
		   = std::less<typename iterator_traits<iter_t>::value_type>  >
inline void indirect_stable_sort(iter_t first, iter_t last, 
                                 compare comp = compare() )
{   //--------------------------------- begin --------------------------------
    bs_algo::indirect_spin_sort(first, last,comp);
};

//
//-----------------------------------------------------------------------------
//  function : paralle_stable_sort
/// @brief this function implement a stable parallel sort. The number of
///        threads to use is defined by the NThread parameter
/// @tparam iter_t : iterators for to access to the elements
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] NT : This object is a integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
//-----------------------------------------------------------------------------
template    < class iter_t >
inline void parallel_stable_sort ( iter_t first, iter_t last ,
                                   NThread NT = NThread() )
{   //---------------------------- begin -------------------------------------
    bs_algo::parallel_stable_sort ( first, last, NT);
};
//
//-----------------------------------------------------------------------------
//  function : paralle_stable_sort
/// @brief this function implement a stable parallel sort. The number of
///        threads to use is defined by the NThread parameter
/// @tparam iter_t : iterators for to access to the elements
/// @tparam compare : object for to compare two elements pointed by the iter_t
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
/// @param [in] NT : This object is a integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
//-----------------------------------------------------------------------------
template    < class iter_t,
              typename compare
			  = std::less < typename iterator_traits<iter_t>::value_type> >
inline void parallel_stable_sort ( iter_t first, iter_t last, compare comp ,
                             NThread NT = NThread() )
{   //---------------------------- begin -------------------------------------
    bs_algo::parallel_stable_sort ( first, last,comp, NT);
};
//
//-----------------------------------------------------------------------------
//  function : sample_sort
/// @brief this function implement a stable parallel sort with the algorithm of
///        sample sort. The number of threads to use is defined by the NThread
///        parameter
/// @tparam iter_t : iterators for to access to the elements
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] NT : This object is a integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
//-----------------------------------------------------------------------------
template    < class iter_t >
inline void sample_sort ( iter_t first, iter_t last , NThread NT = NThread() )
{   //---------------------------- begin -------------------------------------
    bs_algo::sample_sort ( first, last, NT);
};
//
//-----------------------------------------------------------------------------
//  function : sample_sort
/// @brief this function implement a stable parallel sort with the algorithm of
///        sample sort. The number of threads to use is defined by the NThread
///        parameter
/// @tparam iter_t : iterators for to access to the elements
/// @tparam compare : object for to compare two elements pointed by the iter_t
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
/// @param [in] NT : This object is a integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
//-----------------------------------------------------------------------------
template < class iter_t,
           typename compare
		   = std::less<typename iterator_traits<iter_t>::value_type>  >
inline void sample_sort ( iter_t first, iter_t last, compare comp ,
                            NThread NT = NThread() )
{   //---------------------------- begin -------------------------------------
    bs_algo::sample_sort ( first, last,comp, NT);
};

//
//****************************************************************************
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
