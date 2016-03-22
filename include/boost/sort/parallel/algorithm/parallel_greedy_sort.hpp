//----------------------------------------------------------------------------
/// @file parallel_greedy_sort.hpp
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
#ifndef __SORT_ALGORITHM_PARALLEL_GREEDY_SORT_HPP
#define __SORT_ALGORITHM_PARALLEL_GREEDY_SORT_HPP

#include <deque>
#include <memory>
#include <atomic>
#include <iterator>
#include <thread>
#include <vector>
#include <future>
#include <iostream>
#include <boost/sort/parallel/tools/deque_cnc.hpp>
#include <boost/sort/parallel/tools/atomic.hpp>
#include <boost/sort/parallel/algorithm/intro_sort.hpp>
#include <boost/sort/parallel/tools/nthread.hpp>

namespace boost		{
namespace sort		{
namespace parallel	{
namespace algorithm	{
namespace pgs		{


//############################################################################
//                                                                          ##
//                                                                          ##
//                                                                          ##
//                                                                          ##
//                                                                          ##
//############################################################################
namespace bs_tools = boost::sort::parallel::tools;
namespace bs_algo = boost::sort::parallel::algorithm ;

using std::iterator_traits  ;
using bs_tools::NBits64 ;
using bs_algo::intro_sort ;
using bs_algo::deep::pivot9 ;
//using sort::algorithm::deep::divide ;

//------------ work pool -----------------
typedef std::atomic<uint32_t> 					atomic_t ;
typedef typename std::vector<size_t>::iterator  itpos_t ;
//
//############################################################################
//                                                                          ##
//   T K _ M O N O , T K _ D U A L , T K _ S P L I T ; T K _ S O R T        ##
//                                                                          ##
//   T K _ M E R G E , T K _ S I M P L E _ M O V E , T K _ M O V E          ##
//                                                                          ##
//############################################################################
//
//
///---------------------------------------------------------------------------
/// @struct tk_mono
/// @brief
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template < typename value_t>
struct tk_mono
{	//----------------- definitions & variables -----------------------------
	atomic_t 	*pfather ;

	//---------------------------- functions ------------------------------
	tk_mono ( atomic_t *p1 = nullptr ) :pfather (p1){};

	virtual void exec ( void ) = 0;

	virtual ~tk_mono ()
	{	if ( pfather != nullptr ) tools::atomic_sub<uint32_t>( *pfather, 1);
	} ;
};

//
///---------------------------------------------------------------------------
/// @struct tk_split
/// @brief
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template <class sort_alg>
struct tk_sort:public tk_mono<typename sort_alg::value_t >
{	//----------------- definitions ----------------------------
	typedef typename sort_alg::iter_t 	iter_t ;
	typedef typename sort_alg::value_t 	value_t ;
	using tk_mono<value_t>::pfather ;
	//------------------ variables --------------------------
	sort_alg 	&sa ;
	iter_t gfirst , glast ;
	uint32_t 	glevel ;

	//------------------------ functions -----------------------------------
	tk_sort (atomic_t *pf, sort_alg &s1, iter_t first, iter_t last,uint32_t Lv):
		     tk_mono<value_t> (pf),sa(s1),gfirst(first),glast(last),glevel(Lv)
    {};

	void exec ( void )
	{	//------------------------- begin ----------------------------
		sa.sort_thread (pfather, gfirst, glast,glevel);
	};
};
//
//############################################################################
//                                                                          ##
//     U N I Q U E _ P T R      D E F I N I T I O N S                       ##
//                                                                          ##
//     D E Q U E      D E F I N I T I O N S                                 ##
//                                                                          ##
//############################################################################
//
template <typename value_t>
using ptr_tkmono = std::unique_ptr <tk_mono<value_t> > ;

template <typename value_t>
using dqcnc_tkmono = bs_tools::deque_cnc <ptr_tkmono<value_t> >   ;

template <typename value_t>
using dq_tkmono = std::deque <ptr_tkmono<value_t> >   ;

//
///---------------------------------------------------------------------------
/// @struct
/// @brief
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template < typename value_t >
struct dispatcher
{	//------------------- variables ----------------------------
	dqcnc_tkmono <value_t>  dq_mono ;
	//dqcnc_tkdual <value_t>  dq_dual ;
	atomic_t  N ;

	//--------------------- Functions -----------------------------------------
	dispatcher (uint32_t NN = 1):N(NN){};

	bool final ()
	{ 	return ( tools::atomic_read<uint32_t>(N) == 0) ;
	};
	void exec (void  )
	{	ptr_tkmono<value_t> ptkm  ( nullptr) ;

		while ( not final())
		{	if ( dq_mono.pop_move_back( ptkm) )
			{	ptkm->exec() ;
				delete (ptkm.release()) ;
			}
			else  std::this_thread::yield() ;
		};
	};
};
//
//
///---------------------------------------------------------------------------
/// @struct parallel_greedy_sort
/// @brief implement the parallel sort using the intro_sort algorithm
/// @remarks
//----------------------------------------------------------------------------
template < class iter,
           typename compare 
           = std::less <typename iterator_traits<iter>::value_type>  >
struct parallel_greedy_sort
{   //------------------------- begin ----------------------
typedef iter      								iter_t ;
typedef typename iterator_traits<iter_t>::value_type 		value_t ;
typedef parallel_greedy_sort <iter_t, compare> 	this_t ;
typedef tk_sort<this_t> 						tk_sort_t;

//-------------------------------------------------------------------------
//                 Variables
//-------------------------------------------------------------------------
dispatcher<value_t> D ;
size_t MaxThread = 65536;
uint32_t LevelThread = 24 ;
compare comp;

//
//------------------------------------------------------------------------
//  function : parallel_greedy_sort
/// @brief constructor
/// @param [in] first : iterator to the first element to sort
/// @param [in] last : iterator to the next element after the last
//------------------------------------------------------------------------
parallel_greedy_sort (iter_t first, iter_t last, NThread NT  )
                     :parallel_greedy_sort ( first,last, compare(),NT){} ;
//
//------------------------------------------------------------------------
//  function : parallel_greedy_sort
/// @brief constructor of the struct
/// @param [in] first : iterator to the first element to sort
/// @param [in] last : iterator to the next element after the last
/// @param [in] comp : object for to compare
//------------------------------------------------------------------------
parallel_greedy_sort ( iter_t first, iter_t last, compare comp1,
		               NThread NT = NThread() );
//
//------------------------------------------------------------------------
//  function : sort_thread
/// @brief this function is the work asigned to each thread in the parallel
///        process
/// @param [in] first : iterator to the first element to sort
/// @param [in] last : iterator to the next element after the last
/// @param [in] level : level of depth
//------------------------------------------------------------------------
void sort_thread(atomic_t *pf,iter_t first, iter_t last , uint32_t level  );

//----------------------------------------------------------------------------
};//         End class parallel_greedy_sort
//----------------------------------------------------------------------------

//############################################################################
//                                                                          ##
//                                                                          ##
//       N O N    I N L I N E     F U N C T I O N S                         ##
//                                                                          ##
//                                                                          ##
//############################################################################
//
//------------------------------------------------------------------------
//  function : parallel_greedy_sort
/// @brief constructor of the struct
/// @param [in] first : iterator to the first element to sort
/// @param [in] last : iterator to the next element after the last
/// @param [in] comp : object for to compare
//------------------------------------------------------------------------
template < class iter, typename cmp >
parallel_greedy_sort<iter,cmp>::parallel_greedy_sort (iter_t first,iter_t last,
													  cmp comp1, NThread NT)
													  :comp(comp1)
{   //------------------------------- begin ----------------------------------
 	//uint32_t NT = std::thread::hardware_concurrency();
    size_t N = last - first;
    assert ( N >=0);

    uint32_t NBSize = (NBits64(sizeof (value_t)))>>1;
    if ( NBSize > 5 ) NBSize = 5 ;
    MaxThread = 1<< (18 - NBSize);

    uint32_t Level = (NBits64 ( N) <<1);
    LevelThread = (NBits64 ( MaxThread) <<1);

    if ( (size_t)N < MaxThread or NT() == 1 )
    {   deep::intro_sort_internal ( first, last, Level, comp) ;
        return ;
    };

    //------------------- check if sort --------------------------------------
    bool SW = true ;
    for ( iter_t it1 = first, it2 = first+1 ;
        it2 != last and (SW = not comp(*it2,*it1));it1 = it2++);
    if (SW) return ;

    //----------------------------------------------------------------------
    atomic_write ( D.N, 1);
    D.dq_mono.emplace_back ( new tk_sort_t ( &D.N,*this, first,last,Level));
    //------------------------------------------------------------------------
    //                    PROCESS
    //------------------------------------------------------------------------
 	std::vector <std::future <void> > F ( NT() ) ;

   	for ( uint32_t i =0 ; i < NT() ; ++i)
   		F[i] = std::async (std::launch::async, &dispatcher<value_t>::exec,&D);
   	for ( uint32_t i =0 ; i < NT() ; ++i) F[i].get() ;

};

//
//------------------------------------------------------------------------
//  function : sort_thread
/// @brief this function is the work asigned to each thread in the parallel
///        process
/// @param [in] first : iterator to the first element to sort
/// @param [in] last : iterator to the next element after the last
/// @param [in] level : level of depth
//------------------------------------------------------------------------
template < class iter, typename cmp >
void parallel_greedy_sort<iter,cmp>::sort_thread( atomic_t *pf,
                                                  iter_t  first,
		                                          iter_t  last ,
                                                  uint32_t level)
{   //------------------------- begin ----------------------
 	size_t N  = last - first ;

 	if ( N <= MaxThread or level < LevelThread)
    {   intro_sort ( first, last, comp) ;
        return  ;
    };

    //------------------- check if sort --------------------------------------
    bool SW = true ;
    for ( iter_t it1 = first, it2 = first+1 ;
        it2 != last and (SW = not comp(*it2,*it1));it1 = it2++);
    if (SW) return ;

    //-------------------- pivoting  ----------------------------------
    pivot9 ( first, last ,comp);
    const value_t & val = const_cast < value_t &>(* first);
    iter_t c_first = first+1 , c_last  = last-1;

    while ( comp(*c_first, val) ) ++c_first ;
    while ( comp ( val,*c_last ) ) --c_last ;
    while (not( c_first > c_last ))
    {   std::swap ( *(c_first++), *(c_last--));
        while ( comp (*c_first, val) ) ++c_first;
        while ( comp ( val, *c_last) ) --c_last ;
    }; // End while
    std::swap ( *first , * c_last);

    atomic_add ( *pf ,1);
    //atomic_add ( *pf ,2);

	D.dq_mono.emplace_back (new tk_sort_t(pf,*this ,c_first, last, level-1));
	sort_thread (pf,first, c_last, level-1);
    return ;
};
//
//****************************************************************************
};//    End namespace pgs
//****************************************************************************
//
//############################################################################
//                                                                          ##
//                                                                          ##
//         T O P    L E V E L     I N V O C A T I O N S                     ##
//                                                                          ##
//                                                                          ##
//############################################################################
//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief function envelope with the comparison object defined by defect
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the next element to the last valid iterator
//-----------------------------------------------------------------------------
template    < class iter_t >
void parallel_greedy_sort ( iter_t first, iter_t last, NThread NT = NThread()  )
{   pgs::parallel_greedy_sort <iter_t> ( first, last,NT);
};
//
//-----------------------------------------------------------------------------
//  function : parallel_sort
/// @brief function envelope with the comparison object defined by defect
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the next element to the last valid iterator
/// @param [in] comp : object for to compare
//-----------------------------------------------------------------------------
template    < class iter_t,
              typename compare
			  = std::less < typename iterator_traits<iter_t>::value_type> >
void parallel_greedy_sort
( iter_t first, iter_t last, compare comp1, NThread NT = NThread() )
{   pgs::parallel_greedy_sort<iter_t,compare> ( first, last,comp1,NT);
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
