//----------------------------------------------------------------------------
/// @file parallel_sort.hpp
/// @brief Parallel Sort algorithm
///
/// @author Copyright (c) 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_ALGORITHM_PARALLEL_SORT_HPP
#define __BOOST_SORT_PARALLEL_ALGORITHM_PARALLEL_SORT_HPP

#include <iterator>
#include <boost/sort/parallel/tools/atomic.hpp>
#include <boost/sort/parallel/tools/nthread.hpp>
#include <boost/sort/parallel/tools/stack_cnc.hpp>
#include <boost/sort/parallel/algorithm/token.hpp>
#include <boost/sort/parallel/algorithm/intro_sort.hpp>
#include <boost/sort/parallel/algorithm/indirect.hpp>
#include <vector>


namespace boost		{
namespace sort		{
namespace parallel 	{
namespace algorithm {

using std::iterator_traits ;
using boost::sort::parallel::tools::NThread ;
using boost::sort::parallel::tools::NThread_HW ;
//
///---------------------------------------------------------------------------
/// @struct parallel_sort_comp
/// @brief implement the parallel sort using the intro_sort algorithm
/// @tparam iter_t : iterators pointing to the elements
/// @tparam compare : objects for to compare two elements pointed by iter_t
///                   iterators
/// @remarks
//----------------------------------------------------------------------------
template < class iter_t,   
           typename compare  
           = std::less < typename iterator_traits<iter_t>::value_type> >
struct parallel_sort_comp
{
//------------------------- begin ----------------------
typedef typename iterator_traits<iter_t>::value_type	value_t ;
typedef  token_level < iter_t> token_t ;

//-------------------------------------------------------------------------
//                 Variables
//-------------------------------------------------------------------------
stack_cnc <token_t> ST ;
std::atomic<size_t > ND ;
size_t MaxPerThread = 65536;
size_t LevelThread = 32 ;
compare comp;

//
//------------------------------------------------------------------------
//  function : parallel_sort_comp
/// @brief constructor of the struct
/// @param [in] first : iterator to the first element to sort
/// @param [in] last : iterator to the next element after the last
/// @param [in] comp : object for to compare
/// @param [in] NT : variable indicating the number of threads used in the
///                  sorting
//------------------------------------------------------------------------
parallel_sort_comp ( iter_t first, iter_t last,
		             compare comp1, NThread NT= NThread());
//
//------------------------------------------------------------------------
//  function : parallel_sort_comp
/// @brief constructor
/// @param [in] first : iterator to the first element to sort
/// @param [in] last : iterator to the next element after the last
/// @param [in] NT : variable indicating the number of threads used in the
///                  sorting
//------------------------------------------------------------------------
parallel_sort_comp (iter_t first, iter_t last, const NThread &NT= NThread() )
                        :parallel_sort_comp ( first,last, compare(),NT){} ;
//
//------------------------------------------------------------------------
//  function : sort_thread
/// @brief this function is the work asigned to each thread in the parallel
///        process
//------------------------------------------------------------------------
void sort_thread( );

//----------------------------------------------------------------------------
};// end of class parallel_sort
//----------------------------------------------------------------------------
//############################################################################
//                                                                          ##
//          N O N    I N L I N E     F U N C T I O N S                      ##
//                                                                          ##
//############################################################################
//
//
//------------------------------------------------------------------------
//  function : parallel_sort_comp
/// @brief constructor of the struct
/// @param [in] first : iterator to the first element to sort
/// @param [in] last : iterator to the next element after the last
/// @param [in] comp : object for to compare
/// @param [in] NT : variable indicating the number of threads used in the
///                  sorting
//------------------------------------------------------------------------
template < class iter_t,   typename compare  >
parallel_sort_comp<iter_t,compare>::parallel_sort_comp ( iter_t first,
		                                                 iter_t last,
														 compare comp1,
														 NThread NT )
														 :comp(comp1)
{   //------------------------- begin ----------------------
    size_t N = last - first;
    assert ( N >=0);

    uint32_t NBSize = (NBits64(sizeof (value_t)));
    if ( NBSize > 10 ) NBSize = 10 ;
    MaxPerThread = 1<< (19 - NBSize);

    uint32_t Level = (NBits64 ( N) <<1);
    LevelThread = (NBits64 ( MaxPerThread) <<1);

    if ( (size_t)N < MaxPerThread or NT() == 1 )
    {   intro_sort ( first, last, comp) ;
        return ;
    } ;
    //------------------- check if sort ----------------------------------
    bool SW = true ;
    for ( iter_t it1 = first, it2 = first+1 ;
        it2 != last and (SW = not comp(*it2,*it1));it1 = it2++);
    if (SW) return ;

    //-------------------------------------------------------------------
    ST.reserve ( Level * NT() );
    std::atomic_store (&ND, N);
    ST.emplace_back ( first, last, Level);

    std::vector<std::thread> VT (NT());
    for ( uint32_t i  =0  ; i < NT() ; ++i)
        VT[i] = std::thread (&parallel_sort_comp::sort_thread, this);
    for ( uint32_t i  =0  ; i < NT() ; ++i)
        VT[i].join();
};
//
//------------------------------------------------------------------------
//  function : sort_thread
/// @brief this function is the work asigned to each thread in the parallel
///        process
//------------------------------------------------------------------------
template < class iter_t,   typename compare  >
void parallel_sort_comp<iter_t,compare>::sort_thread( )
{   //------------------------- begin ----------------------
    token_t tk ;
    while ( atomic_read ( ND) != 0)
    {   //--------------------------------------------------------------
        //                 read from ST
        //--------------------------------------------------------------
        if ( not ST.pop_copy_back( tk) )
        {   std::this_thread::yield() ;
            continue ;
        };
        size_t N = tk.size() ;
        if ( N <= MaxPerThread or tk.level < LevelThread)
        {   atomic_sub ( ND, N );
            deep::intro_sort_internal ( tk.first, tk.last, tk.level,comp) ;
            continue ;
        };

        //----------------------------------------------------------------
        //                     split
        //----------------------------------------------------------------
        typedef typename iterator_traits<iter_t>::value_type  value_t ;

        //------------------- check if sort ------------------------------
        bool SW = true ;
        for ( iter_t it1 = tk.first, it2 = tk.first+1 ;
              it2 != tk.last and (SW = not comp(*it2,*it1));it1 = it2++);
        if (SW)
        {   atomic_sub ( ND, N );
            continue ;
        };
        //---------------------- pivot select ----------------------------
        deep::pivot9 ( tk.first, tk.last , comp) ;
        value_t &  val = const_cast < value_t &>(* tk.first);
        iter_t c_first = tk.first+1 , c_last  = tk.last-1;

        while ( c_first != tk.last and comp (*c_first, val)) ++c_first ;
        while ( comp(val ,*c_last ) ) --c_last ;
        while (not( c_first > c_last ))
        {   std::swap ( *(c_first++), *(c_last--));
            while ( comp(*c_first,val) ) ++c_first;
            while ( comp(val, *c_last) ) --c_last ;
        }; // End while
        std::swap ( *tk.first , *c_last);

        //----------------------------------------------------------------
        //   Store the tokens en ST
        //----------------------------------------------------------------
        token_t tk1 ( tk.first, c_last, tk.level-1) ;
        token_t tk2 ( c_first , tk.last,  tk.level-1) ;
        auto N1 = (tk1.last - tk1.first);
        auto N2 = (tk2.last - tk2.first);
        auto N3 = N - N1- N2 ;
        if ( N3 != 0) atomic_sub ( ND, N3 );

        ST.push_back ( tk2);
        ST.push_back ( tk1);

    }; // end while
} ;
//############################################################################
//                                                                          ##
//                                                                          ##
//             T O P    L E V E L   I N V O C A T I O N S                   ##
//                                                                          ##
//                                                                          ##
//############################################################################
//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief function envelope with the comparison object defined by defect
///
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the next element to the last valid iterator
/// @param [in] NT : NThread object for to define the number of threads used
///                  in the process. By default is the number of HW threads

//-----------------------------------------------------------------------------
template    < class iter_t >
void parallel_sort ( iter_t first, iter_t last , const NThread &NT = NThread() )
{   parallel_sort_comp <iter_t> ( first, last, NT);
};
//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief function envelope with the comparison object defined by defect
///
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the next element to the last valid iterator
/// @param [in] comp : object for to compare
/// @param [in] NT : NThread object for to define the number of threads used
///                  in the process. By default is the number of HW threads
//-----------------------------------------------------------------------------
template    < class iter_t,
              typename compare = std::less < typename iterator_traits<iter_t>::value_type>
            >
void parallel_sort ( iter_t first, iter_t last, compare comp1 ,
                     const NThread &NT = NThread() )
{   parallel_sort_comp<iter_t,compare> ( first, last,comp1,NT);
};


//############################################################################
//                                                                          ##
//                I N D I R E C T     F U N C T I O N S                     ##
//                                                                          ##
//############################################################################
//
//-----------------------------------------------------------------------------
//  function : indirect_parallel_sort
/// @brief indirect parallel sort
///
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the next element to the last valid iterator
/// @param [in] NT : NThread object for to define the number of threads used
///                  in the process. By default is the number of HW threads
//-----------------------------------------------------------------------------
template < class iter_t >
void indirect_parallel_sort ( iter_t first, iter_t last,
                              const NThread &NT = NThread() )
{   //------------------------------- begin--------------------------
    typedef std::less <typename iterator_traits<iter_t>::value_type> compare ;
    typedef less_ptr_no_null <iter_t, compare>      compare_ptr ;

    std::vector<iter_t> VP ;
    create_index ( first , last , VP);
    parallel_sort  ( VP.begin() , VP.end(), compare_ptr(),NT );
    sort_index ( first , VP) ;
};
//
//-----------------------------------------------------------------------------
//  function : indirect_parallel_sort
/// @brief indirect parallel sort
///
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the next element to the last valid iterator
/// @param [in] comp : object for to compare
/// @param [in] NT : NThread object for to define the number of threads used
///                  in the process. By default is the number of HW threads
//-----------------------------------------------------------------------------
template < class iter_t,
          typename 
          compare = std::less < typename iterator_traits<iter_t>::value_type >
        >
void indirect_parallel_sort ( iter_t first, iter_t last,
                              compare comp1, const NThread &NT = NThread())
{   //----------------------------- begin ----------------------------------
    typedef less_ptr_no_null <iter_t, compare>      compare_ptr ;

    std::vector<iter_t> VP ;
    create_index ( first , last , VP);
    parallel_sort  ( VP.begin() , VP.end(), compare_ptr(comp1),NT );
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
