//----------------------------------------------------------------------------
/// @file sample_sort.hpp
/// @brief Sample Sort algorithm
///
/// @author Copyright (c) 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_DETAIL_SAMPLE_SORT_HPP
#define __BOOST_SORT_PARALLEL_DETAIL_SAMPLE_SORT_HPP

#include <functional>
#include <memory>
#include <type_traits>
#include <iterator>
#include <vector>
#include <future>

#include <algorithm>
#include <boost/sort/parallel/detail/util/atomic.hpp>
#include <boost/sort/parallel/detail/spin_sort.hpp>
#include <boost/sort/parallel/detail/util/range.hpp>
#include <boost/sort/parallel/detail/util/merge_four.hpp>
#include <boost/sort/parallel/detail/util/merge_vector.hpp>

namespace boost		{
namespace sort		{
namespace parallel 	{
namespace detail	{

//
///---------------------------------------------------------------------------
/// @struct sample_sort
/// @brief This a structure for to implement a sample sort, exception
///        safe
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template < class Iter_t,  class  Compare    >
struct sample_sort
{
//------------------------------------------------------------------------
//                     DEFINITIONS
//------------------------------------------------------------------------
typedef typename iterator_traits<Iter_t>::value_type   	value_t ;
typedef range <Iter_t>                      			range_it ;
typedef range <value_t*>                    			range_buf ;
typedef sample_sort<Iter_t, Compare>                    this_t ;
typedef spin_sort<Iter_t,Compare>						spin_sort_t ;

//------------------------------------------------------------------------
//                VARIABLES AND CONSTANTS
//------------------------------------------------------------------------
static const uint32_t thread_min = (1<<16) ;

uint32_t 	nthread, ninterval ;
bool 		construct = false, owner = false  ;
Compare 	comp ;
range_it 	global_range ;
range_buf 	global_buf ;

std::vector <std::vector <range_it > > 	vv_range_it ;
std::vector <std::vector <range_buf> > 	vv_range_buf ;
std::vector <range_it>                  vrange_it_ini ;
std::vector <range_buf>                 vrange_buf_ini ;
std::atomic<uint32_t> 					njob ;


//----------------------------------------------------------------------------
//                       FUNCTIONS OF THE STRUCT
//----------------------------------------------------------------------------
void initial_configuration ( void);

//-----------------------------------------------------------------------------
//                 CONSTRUCTOR AND DESTRUCTOR
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//  function : sample_sort
/// @brief constructor of the class
///
/// @param [in] range_initial : range of objects to sort
/// @param [in] comp : object for to Compare two elements
/// @param [in] nthread : define the number of threads to use
///                  in the process. By default is the number of thread HW
//-----------------------------------------------------------------------------
sample_sort ( Iter_t first, Iter_t last, Compare cmp,
		      uint32_t num_thread, value_t *paux, size_t naux );

sample_sort ( Iter_t first , Iter_t last ):
sample_sort  ( first, last, Compare(),
		       std::thread::hardware_concurrency(), nullptr, 0){};

sample_sort (Iter_t first , Iter_t last , Compare cmp):
sample_sort ( first, last, cmp,
		      std::thread::hardware_concurrency(), nullptr, 0){};

sample_sort ( Iter_t first , Iter_t last , uint32_t num_thread ):
sample_sort  ( first, last, Compare(), num_thread, nullptr, 0){};

sample_sort (Iter_t first,Iter_t last, Compare cmp, uint32_t num_thread):
sample_sort  ( first, last, cmp, num_thread, nullptr, 0){};

//
//-----------------------------------------------------------------------------
//  function : sample_sort
/// @brief constructor of the class
///
/// @param [in] range_initial : range of elements to sort
/// @param [in] comp : object for to Compare two elements
/// @param [in] nthread :define the number of threads to use
///                  in the process. By default is the number of thread HW
/// @param [in] range_buf_initial : range used as auxiliary memory
//-----------------------------------------------------------------------------
sample_sort ( Iter_t first , Iter_t last , Compare cmp,
		      uint32_t num_thread, range_buf range_buf_initial):
sample_sort  ( first, last, cmp, num_thread,
		       range_buf_initial.first, range_buf_initial.size()){};
//-----------------------------------------------------------------------------
//  function :~sample_sort
/// @brief destructor of the class. The utility is to destroy the temporary
///        buffer used in the sorting process
//-----------------------------------------------------------------------------
~sample_sort ( void);

//
//-----------------------------------------------------------------------------
//  function : execute first
/// @brief this a function to assign to each thread in the first merge
//-----------------------------------------------------------------------------
inline void execute_first ( void)
{   //------------------------------- begin ----------------------------------
    uint32_t job =0 ;
    while ((job = util::atomic_add(njob, 1)) < ninterval   )
    {   uninit_merge_level4 ( vrange_buf_ini[job] , vv_range_it[job],
    		                  vv_range_buf[job] ,comp   );
    };
};
//
//-----------------------------------------------------------------------------
//  function : execute
/// @brief this is a function to assignt each thread the final merge
//-----------------------------------------------------------------------------
inline void execute ( void)
{   //------------------------------- begin ----------------------------------
    uint32_t job =0 ;
    while ((job = util::atomic_add(njob, 1)) < ninterval   )
    {   merge_vector4 ( vrange_buf_ini[job] , vrange_it_ini[job] ,
    		            vv_range_buf[job], vv_range_it[job], comp);
    };
};
//
//-----------------------------------------------------------------------------
//  function : first merge
/// @brief Implement the merge of the initially sparse ranges
//-----------------------------------------------------------------------------
inline void first_merge ( void)
{   //---------------------------------- begin -------------------------------
    njob =0 ;
	std::vector <std::future <void> > vfuture ( nthread ) ;

	for ( uint32_t i =0 ; i < nthread ; ++i)
	{	vfuture[i]=std::async (std::launch::async,&this_t::execute_first,this);
	};
	for ( uint32_t i =0 ; i < nthread ; ++i) vfuture[i].get() ;
};
//
//-----------------------------------------------------------------------------
//  function : final merge
/// @brief Implement the final merge of the ranges
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
inline void final_merge ( void)
{   //---------------------------------- begin -------------------------------
    njob =0 ;
	std::vector <std::future <void> > vfuture ( nthread ) ;
	for ( uint32_t i =0 ; i < nthread ; ++i)
	{	vfuture[i] = std::async (std::launch::async, &this_t::execute , this);
	};
	for ( uint32_t i =0 ; i < nthread ; ++i) vfuture[i].get() ;
};

//
//----------------------------------------------------------------------------
};//                    End class sample_sort
//----------------------------------------------------------------------------
//
//
//############################################################################
//                                                                          ##
//              N O N    I N L I N E      F U N C T I O N S                 ##
//                                                                          ##
//                      O F   T H E      C L A S S                          ##
//                                                                          ##
//                   S A M P L E _ S O R T _ T A G                          ##
//                                                                          ##
//############################################################################
//
//-----------------------------------------------------------------------------
//  function : sample_sort
/// @brief constructor of the class
///
/// @param [in] range_initial : range of objects to sort
/// @param [in] comp : object for to Compare two elements
/// @param [in] nthread : NThread object for to define the number of threads to use
///                  in the process. By default is the number of thread HW
//-----------------------------------------------------------------------------
template <class Iter_t, typename Compare>
sample_sort<Iter_t,Compare>::
sample_sort ( Iter_t first, Iter_t last, Compare cmp, uint32_t num_thread,
		      value_t *paux, size_t naux )
            : nthread ( num_thread), owner(false), comp(cmp),
			  global_range(first, last),  global_buf (nullptr,nullptr)
{   //-------------------------- begin -------------------------------------
	assert ( (last - first) >= 0 );
    size_t nelem = size_t ( last - first );
    construct= false ;
    njob = 0 ;


    // Adjust when have many threads and only a few elements
    while (nelem >  thread_min and (nthread * nthread) > (nelem >> 3)  )
    {	nthread /= 2 ;
    };
    ninterval = ( nthread <<3);

    if ( nthread < 2 or nelem <= ( thread_min))
    {   spin_sort<Iter_t, Compare> (first, last, comp);
        return ;
    };

    //------------------- check if sort --------------------------------------
    bool sw = true ;
    for ( Iter_t it1 = first, it2 = first+1 ;
          it2 != last and (sw = not comp(*it2,*it1));it1 = it2++);

    if (sw) return ;

    if ( paux != nullptr)
    {	assert ( naux != 0);
    	global_buf.first = paux ;
    	global_buf.last = paux + naux ;
    	owner = false ;
    }
    else
    {	value_t * ptr = std::get_temporary_buffer<value_t>(nelem).first ;
		if ( ptr == nullptr) throw std::bad_alloc() ;
		owner = true ;
		global_buf = range_buf ( ptr, ptr + nelem);
    };
    //------------------------------------------------------------------------
    //                    PROCESS
    //------------------------------------------------------------------------
    initial_configuration () ;

    first_merge ( );
    construct = true ;
    final_merge ( );
};
//
//-----------------------------------------------------------------------------
//  function :~sample_sort
/// @brief destructor of the class. The utility is to destroy the temporary
///        buffer used in the sorting process
//-----------------------------------------------------------------------------
template <class Iter_t, typename Compare>
sample_sort<Iter_t,Compare>::~sample_sort ( void)
{   //----------------------------------- begin -------------------------
    if ( construct)
    {   destroy ( global_buf);
        construct = false ;
    }
    if ( global_buf.first != nullptr and owner )
    	std::return_temporary_buffer ( global_buf.first) ;
};
//
//-----------------------------------------------------------------------------
//  function : initial_configuration
/// @brief Create the internal data structures, and obtain the inital set of
///        ranges to merge
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template <class Iter_t, typename Compare>
void sample_sort<Iter_t,Compare>::initial_configuration ( void)
{   //--------------------------- begin --------------------------------------
    std::vector <range_it> 	vmem_thread ;
    std::vector <range_buf> vbuf_thread   ;
    size_t nelem = global_range.size() ;

    //------------------------------------------------------------------------
    size_t cupo = nelem  / nthread ;
    Iter_t 		it_first 	= global_range.first ;
    value_t * 	buf_first 	= global_buf.first ;

    for (uint32_t i=0 ; i< nthread-1; ++i, it_first += cupo, buf_first += cupo)
    {	vmem_thread.emplace_back ( it_first, it_first + cupo) ;
        vbuf_thread.emplace_back ( buf_first, buf_first + cupo );
    };

    vmem_thread.emplace_back ( it_first, global_range.last) ;
    vbuf_thread.emplace_back ( buf_first, global_buf.last );

    //------------------------------------------------------------------------
    // Sorting of the ranges
    //------------------------------------------------------------------------
	std::vector <std::future <void> > vfuture ( nthread ) ;

	for ( uint32_t i =0 ; i < nthread ; ++i)
	{	auto func = [=]( ){spin_sort_t(vmem_thread[i].first, vmem_thread[i].last,
            							comp, vbuf_thread[i]); } ;
		vfuture[i] = std::async (std::launch::async,func            );
	};

	for ( uint32_t i =0 ; i < nthread ; ++i) vfuture[i].get() ;

    //------------------------------------------------------------------------
    // Obtain the vector of milestones
    //------------------------------------------------------------------------
    std::vector<Iter_t> vsample;
    vsample.reserve ( nthread * (ninterval-1)) ;

    for ( uint32_t i =0 ; i < nthread ; ++i)
    {   size_t distance = vmem_thread[i].size() / ninterval ;
        for ( size_t j = 1 ,pos = distance; j < ninterval; ++j,pos+=distance)
        {   vsample.push_back (vmem_thread[i].first + pos );
        };
    };
    typedef less_ptr_no_null <Iter_t, Compare>  compare_ptr ;
    typedef typename std::vector<Iter_t>::iterator it_to_it ;
    spin_sort<it_to_it,compare_ptr>  ( vsample.begin() , vsample.end(),
    		                           compare_ptr(comp) );

    //------------------------------------------------------------------------
    // Create the final milestone vector
    //------------------------------------------------------------------------
    std::vector<Iter_t> vmilestone ;
    vmilestone.reserve ( ninterval);

    for ( uint32_t pos =nthread >>1 ; pos < vsample.size() ; pos += nthread )
    {	vmilestone.push_back ( vsample [ pos]);
    };

    //------------------------------------------------------------------------
    // Creation of the first vector of ranges
    //------------------------------------------------------------------------
    std::vector< std::vector<range <Iter_t> > > vv_range_first  (nthread);

    for ( uint32_t i =0 ; i < nthread; ++i)
    {   Iter_t itaux = vmem_thread[i].first ;

        for ( uint32_t k =0 ; k < (ninterval -1) ; ++k)
        {   Iter_t it2 = std::upper_bound ( itaux, vmem_thread[i].last ,
                                            * vmilestone[k], comp );

            vv_range_first[i].emplace_back ( itaux, it2);
            itaux = it2 ;
        };
        vv_range_first[i].emplace_back(itaux,vmem_thread[i].last );
    };

    //------------------------------------------------------------------------
    // Copy in buffer and  creation of the final matrix of ranges
    //------------------------------------------------------------------------
    vv_range_it.resize ( ninterval);
    vv_range_buf.resize ( ninterval);
    vrange_it_ini.reserve (ninterval);
    vrange_buf_ini.reserve (ninterval);

    for ( uint32_t i =0 ; i < ninterval ; ++i)
    {   vv_range_it[i].reserve ( nthread);
        vv_range_buf[i].reserve ( nthread);
    };

    Iter_t it = global_range.first ;
    value_t * it_buf = global_buf.first ;

    for ( uint32_t k =0 ; k < ninterval ; ++k)
    {   size_t nelem_interval =0 ;

        for ( uint32_t i = 0 ; i< nthread ; ++i)
        {   size_t nelem_range = vv_range_first[i][k].size();
            if ( nelem_range != 0 )
            {	vv_range_it[k].push_back(vv_range_first[i][k] );
            };
            nelem_interval += nelem_range ;
        };

        vrange_it_ini.emplace_back (it,it + nelem_interval  );
        vrange_buf_ini.emplace_back (it_buf , it_buf+ nelem_interval) ;

        it += nelem_interval ;
        it_buf += nelem_interval ;
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
