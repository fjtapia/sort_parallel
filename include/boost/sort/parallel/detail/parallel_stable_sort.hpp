//----------------------------------------------------------------------------
/// @file parallel_stable_sort.hpp
/// @brief
///
/// @author Copyright (c) 2010 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_DETAIL_PARALLEL_STABLE_SORT_HPP
#define __BOOST_SORT_PARALLEL_DETAIL_PARALLEL_STABLE_SORT_HPP


#include <functional>
#include <memory>
#include <type_traits>
#include <iterator>
#include <vector>
#include <future>
#include <boost/sort/parallel/detail/sample_sort.hpp>

namespace boost		{
namespace sort		{
namespace parallel	{
namespace detail	{

//
///---------------------------------------------------------------------------
/// @struct parallel_stable_sort
/// @brief This a structure for to implement a parallel stable sort, exception
///        safe
//----------------------------------------------------------------------------
template < class Iter_t, class Compare = util::compare_iter <Iter_t>  >
struct parallel_stable_sort
{
//-------------------------------------------------------------------------
//                      DEFINITIONS
//-------------------------------------------------------------------------
typedef typename std::iterator_traits<Iter_t>::value_type value_t ;

//-------------------------------------------------------------------------
//                     VARIABLES
//-------------------------------------------------------------------------
size_t nelem ;
value_t *ptr ;
const size_t nelem_min = 1<<16 ;

//#########################################################################
//
//                F U N C T I O N S
//
//#########################################################################
//
//-----------------------------------------------------------------------------
//  function : parallel_stable_sort
/// @brief constructor of the class
///
/// @param [in] R : range of elements to sort
/// @param [in] comp : object for to compare two elements
/// @param [in] nthread : define the number of threads to use
///                  in the process. By default is the number of thread HW
//-----------------------------------------------------------------------------
parallel_stable_sort ( Iter_t first , Iter_t last ):
parallel_stable_sort  ( first, last, Compare(),
		                std::thread::hardware_concurrency()){};
//
//-----------------------------------------------------------------------------
//  function : parallel_stable_sort
/// @brief envelope function for to call to a parallel_stable_sort object
///
/// @param [in] first : iterator to the first element of the range
/// @param [in] last : iterator to next element after the last of the range
/// @param [in] nthread : NThread object for to define the number of threads to use
///                  in the process. By default is the number of thread HW
//-----------------------------------------------------------------------------
parallel_stable_sort (Iter_t first , Iter_t last , Compare cmp):
parallel_stable_sort (first, last, cmp, std::thread::hardware_concurrency()){};

parallel_stable_sort ( Iter_t first , Iter_t last , uint32_t num_thread):
parallel_stable_sort  ( first, last, Compare(), num_thread){};

parallel_stable_sort (Iter_t first, Iter_t last, Compare cmp,
		              uint32_t num_thread);

//
//-----------------------------------------------------------------------------
//  function :~parallel_stable_sort
/// @brief destructor of the class. The utility is to destroy the temporary
///        buffer used in the sorting process
//-----------------------------------------------------------------------------
~parallel_stable_sort ()
{   //------------------------------- begin ----------------------------------
    if ( ptr != nullptr) std::return_temporary_buffer ( ptr) ;
};
//----------------------------------------------------------------------------
};// end struct parallel_stable_sort
//----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//  function : parallel_stable_sort
/// @brief constructor of the class
///
/// @param [in] range_initial : range of elements to sort
/// @param [in] comp : object for to compare two elements
/// @param [in] nthread : define the number of threads to use
///                  in the process. By default is the number of thread HW
//-----------------------------------------------------------------------------
template < class Iter_t, class Compare >
parallel_stable_sort<Iter_t,Compare>::
parallel_stable_sort ( Iter_t first , Iter_t last , Compare comp,
                       uint32_t  nthread ):nelem(0),ptr(nullptr)
{   //------------------------------- begin -----------------------------
	util::range<Iter_t> range_initial ( first , last ) ;
	assert ( range_initial.valid());

    nelem = range_initial.size() ;
    size_t nptr = ( nelem +1 )>>1 ;

    if ( nelem < nelem_min or nthread < 2)
    {   spin_sort<Iter_t,Compare>(range_initial.first,range_initial.last,comp);
        return ;
    };
    //------------------- check if sort --------------------------------------
    bool sw = true ;
    for ( Iter_t it1 = range_initial.first, it2 = range_initial.first+1 ;
          it2 != range_initial.last and (sw = not comp(*it2,*it1));it1 = it2++);
    if (sw) return ;

    ptr = std::get_temporary_buffer<value_t>(nptr).first ;
    if ( ptr == nullptr) throw std::bad_alloc();

    //---------------------------------------------------------------------
    //     Parallel Process
    //---------------------------------------------------------------------
    range<Iter_t> range_first (range_initial.first, range_initial.first+nptr);
    range<Iter_t> range_second(range_initial.first+nptr, range_initial.last );
    range <value_t*> range_buffer (ptr , ptr + nptr );

    sample_sort<Iter_t,Compare> (range_initial.first, range_initial.first+nptr,
    		                     comp, nthread, range_buffer );

    sample_sort<Iter_t,Compare> (range_initial.first+nptr, range_initial.last,
    		                     comp, nthread, range_buffer);

    range_buffer = init_move (range_buffer, range_first);
    range_initial = half_merge (range_initial,range_buffer,range_second,comp);
}; // end of constructor

//
//****************************************************************************
};//    End namespace algorithm
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
