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
#include <boost/sort/parallel/detail/util/compare_traits.hpp>
#include <boost/sort/parallel/detail/parallel_stable_sort.hpp>
#include <boost/sort/parallel/detail/block_indirect_sort.hpp>


namespace boost 	{
namespace sort		{
namespace parallel	{

//
//****************************************************************************
//             USING AND DEFINITIONS
//****************************************************************************
namespace bpd = boost::sort::parallel::detail ;


//----------------------------------------------------------------------------
using std::iterator_traits ;
using bpd::less_ptr_no_null;
using bpd::util::compare_iter ;
using bpd::util::comp_unknown ;

//
//-----------------------------------------------------------------------------
//  function : sort
/// @brief this function implement a non stable sort, based internally in the
///        intro_sort algorithm. Run with 1 thread
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
//-----------------------------------------------------------------------------
template < class Iter_t,  typename Compare = compare_iter<Iter_t>  >
void sort ( Iter_t first, Iter_t last,Compare comp = Compare())
{   //------------------------- begin ----------------------
	bpd::intro_sort ( first, last, comp) ;
};
//
//-----------------------------------------------------------------------------
//  function : indirect_intro_sort
/// @brief : function for to implement an indirect sort
///
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the element after the last in the range
/// @param [in] comp : object for to Compare elements
//-----------------------------------------------------------------------------
template < class Iter_t, typename Compare = compare_iter<Iter_t>  >
void indirect_sort (Iter_t first, Iter_t last, Compare comp = Compare())
{   //------------------------------- begin--------------------------
    typedef less_ptr_no_null <Iter_t, Compare>      compare_ptr ;

    std::vector<Iter_t> v_iter ;
    bpd::create_index ( first , last , v_iter);
    bpd::intro_sort  ( v_iter.begin() , v_iter.end(), compare_ptr(comp) );
    bpd::sort_index ( first , v_iter) ;
};
//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief this function implement a non stable parallel sort.
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
/// @param [in] nthread : integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
//-----------------------------------------------------------------------------
template < class Iter_t,   class Compare   >
void parallel_sort ( Iter_t first, Iter_t last,Compare comp, uint32_t nthread )
{   //----------------------------- begin ----------------------------------
    bpd::block_indirect_sort<1024, 64,Iter_t,Compare> ( first, last,comp,nthread);
};
//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief this function implement a non stable parallel sort. The number of
///        threads to use is HW threads of the machine
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
//-----------------------------------------------------------------------------
template < class Iter_t, class Compare = compare_iter<Iter_t> >
void parallel_sort ( Iter_t first, Iter_t last )
{   //----------------------------------- begin ------------------------------
    bpd::block_indirect_sort<1024, 64,Iter_t,Compare> ( first, last);
};
//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief this function implement a non stable parallel sort.
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] ukn : If this patrameter is an integer, represent the number
///                   of threads to use in the sorting, otherwise, represent
///                   the comparison object
//-----------------------------------------------------------------------------
template < class Iter_t , class unknown>
void parallel_sort ( Iter_t first, Iter_t last , unknown ukn )
{   //----------------------------------- begin ------------------------------
	typedef typename comp_unknown<Iter_t, unknown>::value_type  comparator ;
	bpd::block_indirect_sort<1024, 64,Iter_t,comparator> ( first, last,ukn);
};

//
//-----------------------------------------------------------------------------
//  function : stable_sort
/// @brief this function implement a single thread stable sort
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
//-----------------------------------------------------------------------------
template < class Iter_t, class Compare = compare_iter<Iter_t> >
void stable_sort (Iter_t first, Iter_t last, Compare comp = Compare() )
{   //----------------------------- begin ----------------------------
	bpd::spin_sort<Iter_t,Compare> ( first, last , comp);
};
//
//-----------------------------------------------------------------------------
//  function : indirect_stable_sort
/// @brief this function implement a single thread indirect_stable sort
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
//-----------------------------------------------------------------------------
template < class Iter_t, class Compare = compare_iter<Iter_t>  >
void indirect_stable_sort (Iter_t first, Iter_t last, Compare comp=Compare())
{   //------------------------------- begin-----------------------------------
    typedef less_ptr_no_null <Iter_t, Compare>      compare_ptr ;
    typedef typename std::vector<Iter_t>::iterator  iter_ptr ;

    std::vector<Iter_t> v_iter ;
    bpd::create_index ( first , last , v_iter);
    bpd::spin_sort<iter_ptr,compare_ptr>  ( v_iter.begin() , v_iter.end(),
    		                                compare_ptr(comp) );

    bpd::sort_index ( first , v_iter) ;
};
//
//-----------------------------------------------------------------------------
//  function : parallel_stable_sort
/// @brief invocation to the parallel stable sort algorithm. The number of
///        threads to use is HW threads of the machine
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
//-----------------------------------------------------------------------------
template < class Iter_t, class Compare = compare_iter<Iter_t>  >
void parallel_stable_sort ( Iter_t first, Iter_t last )
{   //----------------------------------- begin ------------------------------
    bpd::parallel_stable_sort <Iter_t,Compare> ( first, last );
};
//
//-----------------------------------------------------------------------------
//  function : parallel_stable_sort
/// @brief this function implement a parallel stable sort.
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] ukn : If this patrameter is an integer, represent the number
///                   of threads to use in the sorting, otherwise, represent
///                   the comparison object
//-----------------------------------------------------------------------------
template < class Iter_t , class unknown>
void parallel_stable_sort ( Iter_t first, Iter_t last , unknown ukn )
{   //----------------------------------- begin ------------------------------
	//typedef typename iterator_traits<Iter_t>::value_type 	value_t ;
	typedef typename comp_unknown<Iter_t, unknown>::value_type  comparator ;

	bpd::parallel_stable_sort <Iter_t,comparator> ( first , last , ukn);
};
//
//-----------------------------------------------------------------------------
//  function : parallel_stable_sort
/// @brief invocation to the parallel block_indirect_sort algorithm
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
/// @param [in] nthread : integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
//-----------------------------------------------------------------------------
template < class Iter_t,   typename Compare      >
void parallel_stable_sort ( Iter_t first, Iter_t last,
                           Compare comp, uint32_t nthread )
{   //----------------------------- begin ----------------------------------
    bpd::parallel_stable_sort <Iter_t,Compare> ( first, last, comp, nthread);
};
//
//-----------------------------------------------------------------------------
//  function : sample_sort
/// @brief invocation to the parallel stable sort algorithm. The number of
///        threads to use is HW threads of the machine
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
//-----------------------------------------------------------------------------

template < class Iter_t , class Compare = compare_iter<Iter_t> >
void sample_sort ( Iter_t first, Iter_t last )
{   //----------------------------------- begin ------------------------------
    bpd::sample_sort <Iter_t,Compare> ( first, last );
};
//
//-----------------------------------------------------------------------------
//  function : sample_sort
/// @brief this function implement a parallel stable sort by the sample sort
///         algorithm
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] ukn : If this patrameter is an integer, represent the number
///                   of threads to use in the sorting, otherwise, represent
///                   the comparison object
//-----------------------------------------------------------------------------
template < class Iter_t , class unknown>
void sample_sort ( Iter_t first, Iter_t last , unknown ukn )
{   //----------------------------------- begin ------------------------------
	//typedef typename iterator_traits<Iter_t>::value_type 	value_t ;
	typedef typename comp_unknown<Iter_t, unknown>::value_type  comparator ;

	bpd::sample_sort <Iter_t,comparator> ( first , last , ukn);
};
//
//-----------------------------------------------------------------------------
//  function : sample_sort
/// @brief this function implement a parallel stable sort by the sample sort
///         algorithm
///
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
/// @param [in] nthread : integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
//-----------------------------------------------------------------------------
template < class Iter_t,   class Compare      >
void sample_sort ( Iter_t first, Iter_t last, Compare comp, uint32_t nthread )
{   //----------------------------- begin ----------------------------------
    bpd::sample_sort <Iter_t,Compare> ( first, last, comp , nthread ) ;
};
//
//****************************************************************************
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
