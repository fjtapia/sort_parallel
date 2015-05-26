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
#ifndef __BOOST_SORT_GENERAL_SORT_HPP
#define __BOOST_SORT_GENERAL_SORT_HPP

#include <boost/sort/parallel/algorithm/parallel_sort.hpp>
#include <boost/sort/parallel/algorithm/parallel_stable_sort.hpp>



namespace boost
{
namespace sort
{
namespace parallel
{

//****************************************************************************
//             USING AND DEFINITIONS
//****************************************************************************
namespace bs_algo = boost::sort::parallel::algorithm ;
namespace bspu = boost::sort::parallel::util;
using bspu::NThread ;
using bspu::NThread_HW ;
using bspu::iter_value ;
//----------------------------------------------------------------------------
// The code of the class NThread is in boost/sort/parallel/util/atomic.hpp
//----------------------------------------------------------------------------


//
//-----------------------------------------------------------------------------
//  function : introsort
/// @brief this function implement a non stable sort, based internally in the
///        intro_sort algorithm. Run with 1 thread
/// @tparam iter_t : iterators for to access to the elements
/// @tparam compare : object for to compare two elements pointed by the iter_t
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] comp : object for to compare two elements pointed by iter_t
///                    iterators
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less <typename iter_value<iter_t>::type> >
inline void introsort ( iter_t first, iter_t last,compare comp = compare())
{   //---------------------------- begin -------------------------------------
    bs_algo::intro_sort(first, last,comp);
};
//
//-----------------------------------------------------------------------------
//  function : paralle_introsort
/// @brief this function implement a non stable parallel sort. The number of
///        threads to use is defined by the NThread parameter
/// @tparam iter_t : iterators for to access to the elements
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] NT : This object is a integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template    < class iter_t >
inline void parallel_introsort ( iter_t first, iter_t last , const NThread &NT = NThread() )
{   //---------------------------- begin -------------------------------------
    bs_algo::parallel_sort ( first, last, NT);
};
//
//-----------------------------------------------------------------------------
//  function : paralle_introsort
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
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less <typename iter_value<iter_t>::type> >
inline void parallel_introsort ( iter_t first, iter_t last,
                            compare comp, const NThread &NT = NThread() )
{   //---------------------------- begin -------------------------------------
    bs_algo::parallel_sort ( first, last,comp, NT);
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
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less<typename iter_value<iter_t>::type>
         >
inline void smart_merge_sort(iter_t first, iter_t last, compare comp = compare() )
{   //--------------------------------- begin --------------------------------
    typedef typename iter_value<iter_t>::type   value_t ;
    if ( sizeof ( value_t) > 128)
        bs_algo::indirect_smart_merge_sort(first, last,comp);
    else
        bs_algo::smart_merge_sort(first, last,comp);
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
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template    < class iter_t >
inline void parallel_stable_sort ( iter_t first, iter_t last ,
                                   const NThread &NT = NThread() )
{   //---------------------------- begin -------------------------------------
    typedef typename iter_value<iter_t>::type   value_t ;
    if ( sizeof ( value_t) > 64)
        bs_algo::indirect_parallel_stable_sort ( first, last, NT);
    else
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
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template    < class iter_t,
              typename compare = std::less < typename iter_value<iter_t>::type>
            >
inline void parallel_stable_sort ( iter_t first, iter_t last, compare comp ,
                             const NThread &NT = NThread() )
{   //---------------------------- begin -------------------------------------
    typedef typename iter_value<iter_t>::type   value_t ;
    if ( sizeof ( value_t) > 64)
        bs_algo::indirect_parallel_stable_sort ( first, last,comp, NT);
    else
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
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template    < class iter_t >
inline void sample_sort ( iter_t first, iter_t last , const NThread &NT = NThread() )
{   //---------------------------- begin -------------------------------------
    typedef typename iter_value<iter_t>::type   value_t ;
    if ( sizeof ( value_t) > 64)
        bs_algo::indirect_sample_sort ( first, last, NT);
    else
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
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template    < class iter_t,
              typename compare = std::less < typename iter_value<iter_t>::type>
            >
inline void sample_sort ( iter_t first, iter_t last, compare comp ,
                            const NThread &NT = NThread() )
{   //---------------------------- begin -------------------------------------
    typedef typename iter_value<iter_t>::type   value_t ;
    if ( sizeof ( value_t) > 64)
        bs_algo::indirect_sample_sort ( first, last,comp, NT);
    else
        bs_algo::sample_sort ( first, last,comp, NT);
};

//
//****************************************************************************
};//    End namespace parallelsort
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
