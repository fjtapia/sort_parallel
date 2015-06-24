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

#include <boost/sort/parallel/util/atomic.hpp>
#include <boost/sort/parallel/util/nthread.hpp>
#include <boost/sort/parallel/util/stack_cnc.hpp>
#include <boost/sort/parallel/algorithm/intro_sort.hpp>
#include <boost/sort/parallel/algorithm/indirect.hpp>
#include <vector>


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
/// @struct parallel_sort_comp
/// @brief implement the parallel sort using the intro_sort algorithm
/// @tparam iter_t : iterators pointing to the elements
/// @tparam compare : objects for to compare two elements pointed by iter_t
///                   iterators
/// @remarks
//----------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less <typename iter_value<iter_t>::type>  >
struct parallel_sort_comp
{   //------------------------- begin ----------------------
    typedef  token_level < iter_t> token_t ;
    size_type MaxPerThread = 65536;
    static const size_t Min_Parallel = 65536 ;
    compare comp;

    //-------------------------------------------------------------------------
    //                 Variables
    //-------------------------------------------------------------------------
    stack_cnc <token_t> ST ;
    std::atomic<difference_type > ND ;

    //
    //------------------------------------------------------------------------
    //  function : parallel_sort_comp
    /// @brief constructor
    /// @param [in] first : iterator to the first element to sort
    /// @param [in] last : iterator to the next element after the last
    /// @param [in] NT : variable indicating the number of threads used in the
    ///                  sorting
    /// @exception
    /// @return
    /// @remarks
    //------------------------------------------------------------------------
    parallel_sort_comp (iter_t first, iter_t last, const NThread &NT= NThread() )
                        :parallel_sort_comp ( first,last, compare(),NT){} ;
    //
    //------------------------------------------------------------------------
    //  function : parallel_sort_comp
    /// @brief constructor of the struct
    /// @param [in] first : iterator to the first element to sort
    /// @param [in] last : iterator to the next element after the last
    /// @param [in] comp : object for to compare
    /// @param [in] NT : variable indicating the number of threads used in the
    ///                  sorting
    /// @exception
    /// @return
    /// @remarks
    //------------------------------------------------------------------------
    parallel_sort_comp ( iter_t first, iter_t last, compare comp1,
                         const NThread &NT= NThread()):comp(comp1)
    {   //------------------------- begin ----------------------
        difference_type N = last - first;
        assert ( N >=0);
        uint32_t Level = (NBits (N)<<1)  ;
        MaxPerThread = N / (NT()<<5);

        if ( (size_t)N < Min_Parallel or NT() == 1 )
        {   intro_sort_internal ( first, last, Level,comp) ;
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
    /// @exception
    /// @return
    /// @remarks
    //------------------------------------------------------------------------
    void sort_thread( )
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
            difference_type N = tk.last - tk.first ;
            if ( (size_type)N <= MaxPerThread or tk.level == 0)
            {   atomic_sub ( ND, N );
                intro_sort_internal ( tk.first, tk.last, tk.level,comp) ;
                continue ;
            };

            //----------------------------------------------------------------
            //                     split
            //----------------------------------------------------------------
            typedef typename iter_value<iter_t>::type  value_t ;

            //------------------- check if sort ------------------------------
            bool SW = true ;
            for ( iter_t it1 = tk.first, it2 = tk.first+1 ;
                  it2 != tk.last and (SW = not comp(*it2,*it1));it1 = it2++);
            if (SW)
            {   atomic_sub ( ND, N );
                continue ;
            };
            //---------------------- pivot select ----------------------------
            size_t Nx = ( size_t (N ) >>1 ) ;

            iter_t itA = tk.first +1 ;
            iter_t itB = tk.first + Nx ;
            iter_t itC = tk.last -1 ;

            if ( comp( *itB , *itA )) std::swap ( *itA, *itB);
            if ( comp (*itC , *itB))
            {   std::swap (*itC , *itB );
                if ( comp( *itB , *itA )) std::swap ( *itA, *itB);
            };
            std::swap ( *tk.first , *itB);
            value_t &  val = const_cast < value_t &>(* tk.first);
            iter_t c_first = tk.first+2 , c_last  = tk.last-2;

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
            if ( N2 > N1 )
            {   ST.push_back ( tk2);
                ST.push_back ( tk1);
            }
            else
            {   ST.push_back ( tk1);
                ST.push_back ( tk2);
            } ;
        }; // end while
    } ;
};

//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief function envelope with the comparison object defined by defect
/// @tparam iter_t : iterator used for to access to the elements
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the next element to the last valid iterator
/// @param [in] NT : NThread object for to define the number of threads used
///                  in the process. By default is the number of HW threads
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template    < class iter_t >
void parallel_sort ( iter_t first, iter_t last , const NThread &NT = NThread() )
{   parallel_sort_comp <iter_t> ( first, last, NT);
};
//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief function envelope with the comparison object defined by defect
/// @tparam iter_t : iterator used for to access to the elements
/// @tparam compare : object for to compare the lements pointed by iter_t
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the next element to the last valid iterator
/// @param [in] comp : object for to compare
/// @param [in] NT : NThread object for to define the number of threads used
///                  in the process. By default is the number of HW threads
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template    < class iter_t,
              typename compare = std::less < typename iter_value<iter_t>::type>
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
/// @tparam iter_t : iterator used for to access to the elements
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the next element to the last valid iterator
/// @param [in] NT : NThread object for to define the number of threads used
///                  in the process. By default is the number of HW threads
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t >
void indirect_parallel_sort ( iter_t first, iter_t last, 
                              const NThread &NT = NThread() )
{   //------------------------------- begin--------------------------
    typedef std::less <typename iter_value<iter_t>::type> compare ;
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
/// @tparam iter_t : iterator used for to access to the elements
/// @tparam compare : object for to compare the lements pointed by iter_t
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the next element to the last valid iterator
/// @param [in] comp : object for to compare
/// @param [in] NT : NThread object for to define the number of threads used
///                  in the process. By default is the number of HW threads
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t,
          typename compare = std::less < typename iter_value<iter_t>::type >
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
