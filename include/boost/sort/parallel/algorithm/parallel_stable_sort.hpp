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
#ifndef __CNTREE_SORT_PARALLEL_STABLE_SORT_HPP
#define __CNTREE_SORT_PARALLEL_STABLE_SORT_HPP

#include <boost/sort/parallel/util/atomic.hpp>
#include <boost/sort/parallel/util/nthread.hpp>
#include <boost/sort/parallel/util/stack_cnc.hpp>
#include <boost/sort/parallel/algorithm/sample_sort.hpp>
#include <functional>
#include <vector>
#include <boost/sort/parallel/algorithm/indirect.hpp>
#include <iostream>

namespace boost
{
namespace sort
{
namespace parallel
{
namespace algorithm
{
namespace bspu = boost::sort::parallel::util;
using bspu::NThread ;
using bspu::NThread_HW ;
//
///---------------------------------------------------------------------------
/// @struct parallel_stable_sort_tag
/// @brief This a structure for to implement a parallel stable sort, exception
///        safe
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template < class iter_t,
          typename compare = std::less < typename iter_value<iter_t>::type >
        >
struct parallel_stable_sort_tag
{
//-------------------------------------------------------------------------
//                      DEFINITIONS
//-------------------------------------------------------------------------
typedef typename iter_value<iter_t>::type value_t ;

//-------------------------------------------------------------------------
//                     VARIABLES
//-------------------------------------------------------------------------
size_t NElem ;
value_t *Ptr ;
const size_t NELEM_MIN = 1<<16 ;

//#########################################################################
//
//                F U N C T I O N S
//
//#########################################################################
//
//-----------------------------------------------------------------------------
//  function : parallel_stable_sort_tag
/// @brief constructor of the class
/// @param [in] first : iterator to the first element of the range
/// @param [in] last : iterator to next element after the last of the range
/// @param [in] comp : object for to compare two elements
/// @param [in] NT : NThread object for to define the number of threads to use
///                  in the process. By default is the number of thread HW
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
parallel_stable_sort_tag ( iter_t first, iter_t last, compare comp,
                           const NThread &NT=NThread() ):NElem(0),Ptr(nullptr)
{   //------------------------------- begin -----------------------------
    auto  N1 = last -first;
    assert ( N1 >= 0 );
    NElem = (size_t) N1 ;
    size_t NPtr = ( NElem +1 )>>1 ;

    if ( NElem < NELEM_MIN or NT() == 1)
    {   smart_merge_sort ( first , last, comp );
        return ;
    };
    //------------------- check if sort --------------------------------------
    bool SW = true ;
    for ( iter_t it1 = first, it2 = first+1 ;
          it2 != last and (SW = not comp(*it2,*it1));it1 = it2++);
    if (SW) return ;

    Ptr = std::get_temporary_buffer<value_t>(NPtr).first ;
    if ( Ptr == nullptr) throw std::bad_alloc();
    //---------------------------------------------------------------------
    //     Parallel Process
    //---------------------------------------------------------------------
    sample_sort_tag<iter_t, compare> ( first, first + NPtr, comp,NT, Ptr, NPtr);
    sample_sort_tag<iter_t, compare> ( first + NPtr, last , comp, NT,Ptr, NPtr);

    move_buf (Ptr, first, first+NPtr );
    half_merge (Ptr, Ptr+NPtr, first+NPtr, last, first, comp );
}; // end of constructor
//
//-----------------------------------------------------------------------------
//  function :~parallel_stable_sort_tag
/// @brief destructor of the class. The utility is to destroy the temporary
///        buffer used in the sorting process
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
~parallel_stable_sort_tag ()
{   //------------------------------- begin ----------------------------------
    if ( Ptr != nullptr) std::return_temporary_buffer ( Ptr) ;
};
//----------------------------------------------------------------------------
};// end struct parallel_stable_sort_tag
//----------------------------------------------------------------------------

//
//-----------------------------------------------------------------------------
//  function : parallel_stable_sort
/// @brief envelope function for to call to a parallel_stable_sort_tag object
/// @tparam iter_t : iterator used for to access to the data
/// @param [in] first : iterator to the first element of the range
/// @param [in] last : iterator to next element after the last of the range
/// @param [in] NT : NThread object for to define the number of threads to use
///                  in the process. By default is the number of thread HW
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t >
void parallel_stable_sort ( iter_t first, iter_t last , 
                            const NThread &NT = NThread() )
{   //------------------------------- begin ----------------------------------
    typedef std::less<typename iter_value<iter_t>::type > compare;
    parallel_stable_sort_tag <iter_t,compare> ( first, last,compare(), NT);
};
//
//-----------------------------------------------------------------------------
//  function : parallel_stable_sort
/// @brief envelope function for to call to a parallel_stable_sort_tag object
/// @tparam iter_t : iterator used for to access to the data
/// @tparam compare : object for to compare two elements
/// @param [in] first : iterator to the first element of the range
/// @param [in] last : iterator to next element after the last of the range
/// @param [in] comp : object for to compare two elements
/// @param [in] NT : NThread object for to define the number of threads to use
///                  in the process. By default is the number of thread HW
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t,
          typename compare = std::less <typename iter_value<iter_t>::type> >
void parallel_stable_sort ( iter_t first, iter_t last,
                           compare comp1, const NThread &NT = NThread() )
{   //----------------------------- begin ----------------------------------
    parallel_stable_sort_tag<iter_t,compare> ( first, last,comp1,NT);
};

//############################################################################
//                                                                          ##
//                I N D I R E C T     F U N C T I O N S                     ##
//                                                                          ##
//############################################################################
//
//-----------------------------------------------------------------------------
//  function : indirect_parallel_stable_sort
/// @brief indirect sorting using the parallel_stable_sort algorithm
/// @tparam iter_t : iterator used for to access to the data
/// @param [in] first : iterator to the first element of the range
/// @param [in] last : iterator to next element after the last of the range
/// @param [in] NT : NThread object for to define the number of threads to use
///                  in the process. By default is the number of thread HW
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t >
void indirect_parallel_stable_sort ( iter_t first, iter_t last ,
                                      const NThread &NT = NThread() )
{   //------------------------------- begin--------------------------
    typedef std::less <typename iter_value<iter_t>::type> compare ;
    typedef less_ptr_no_null <iter_t, compare>      compare_ptr ;

    std::vector<iter_t> VP ;
    create_index ( first , last , VP);
    parallel_stable_sort  ( VP.begin() , VP.end(), compare_ptr(),NT );
    sort_index ( first , VP) ;
};
//
//-----------------------------------------------------------------------------
//  function : indirect_parallel_stable_sort
/// @brief indirect sorting using the parallel_stable_sort algorithm
/// @tparam iter_t : iterator used for to access to the data
/// @tparam compare : object for to compare two elements
/// @param [in] first : iterator to the first element of the range
/// @param [in] last : iterator to next element after the last of the range
/// @param [in] comp : object for to compare two elements
/// @param [in] NT : NThread object for to define the number of threads to use
///                  in the process. By default is the number of thread HW
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t,
          typename compare = std::less <typename iter_value<iter_t>::type> >
void indirect_parallel_stable_sort ( iter_t first, iter_t last,
                                    compare comp1, const NThread & NT = NThread() )
{   //----------------------------- begin ----------------------------------
    typedef less_ptr_no_null <iter_t, compare>      compare_ptr ;

    std::vector<iter_t> VP ;
    create_index ( first , last , VP);
    parallel_stable_sort  ( VP.begin() , VP.end(), compare_ptr(comp1),NT );
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
