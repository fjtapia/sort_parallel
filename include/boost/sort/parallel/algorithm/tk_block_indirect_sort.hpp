//----------------------------------------------------------------------------
 /// @file tk_parallel_block_indirect_block.hpp
/// @brief token data strucutres for the parallel block indirect sort algorithm
///
/// @author Copyright (c) 2010 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __SORT_ALGORITHM_TK_BLOCK_INDIRECT_SORT_HPP
#define __SORT_ALGORITHM_TK_BLOCK_INDIRECT_SORT_HPP

#include <deque>
#include <memory>
#include <atomic>
#include <thread>
#include <iostream>
#include <boost/sort/parallel/tools/deque_cnc.hpp>
#include <boost/sort/parallel/tools/atomic.hpp>
#include <boost/sort/parallel/algorithm/intro_sort.hpp>
#include <boost/sort/parallel/util/range.hpp>

namespace boost		{
namespace sort		{
namespace parallel	{
namespace algorithm	{
namespace pbs		{
//
//############################################################################
//                                                                          ##
//                                                                          ##
//                                                                          ##
//                                                                          ##
//                                                                          ##
//############################################################################
namespace bs_tools 	= boost::sort::parallel::tools ;
namespace bs_util 	= boost::sort::parallel::util ;
namespace bs_algo 	= boost::sort::parallel::algorithm;

using bs_util::range ;
using bs_tools::NBits64 ;
using bs_algo::intro_sort ;


//------------ work pool -----------------
typedef range <size_t>							range_pos ;
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
///---------------------------------------------------------------------------
/// @struct tk_mono
/// @brief
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template < typename value_t>
struct tk_mono
{	//----------------- definitions & variables -----------------------------
	typedef range <value_t*>  	range_buf ;
	atomic_t 					*pfather ;

	//---------------------------- functions ------------------------------
	tk_mono ( atomic_t *p1 = nullptr ) :pfather (p1){};

	virtual void exec ( range_buf RB ) = 0;

	virtual ~tk_mono ()
	{	if ( pfather != nullptr ) bs_tools::atomic_sub<uint32_t>( *pfather, 1);
	} ;
};
//
///---------------------------------------------------------------------------
/// @struct tk_sort
/// @brief
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template <class sort_alg>
struct tk_sort:public tk_mono<typename sort_alg::value_t >
{	//----------------- definitions ----------------------------
	typedef typename sort_alg::iter_t 	iter_t ;
	typedef typename sort_alg::value_t 	value_t ;
	typedef range <value_t*>  			range_buf ;

	using tk_mono<value_t>::pfather ;
	//------------------ variables --------------------------
	sort_alg 	&sa ;
	iter_t gfirst , glast ;
	uint32_t 	glevel ;

	//------------------------ functions -----------------------------------
	tk_sort ( atomic_t *pf, sort_alg & s1 , 
              iter_t first , iter_t last ,uint32_t Lv)
            : tk_mono<value_t> ( pf),sa(s1),gfirst(first),glast(last),glevel(Lv)
    { };
             
	void exec ( range_buf )
	{	//------------------------- begin ----------------------------
		sa.sort_thread (pfather, gfirst, glast,glevel);
	};
};
//
///---------------------------------------------------------------------------
/// @struct
/// @brief
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template < class sort_alg >
struct tk_merge:public tk_mono<typename sort_alg::value_t>
{	//----------------- definitions ----------------------------
	typedef typename sort_alg::value_t 	value_t ;
	typedef range <value_t*>  			range_buf ;

	//-------------------- variables -----------------------
	sort_alg &sa ;
	range_pos RP ;

	//------------------------- functions -------------------------
	tk_merge ( atomic_t *pf , sort_alg &s1, range_pos R1 ):
		      tk_mono<value_t> ( pf),sa ( s1), RP (R1) {} ;

	void exec ( range_buf Rbuf)
	{	//----------------------- begin ------------------------------------
		if (RP.size() < 2 ) return  ;
		for ( size_t k = RP.first +1 ; k < RP.last ; ++k)
		{	in_place_merge_uncontiguous ( sa.rng_pos ( sa.V[k-1].pos),
				                          sa.rng_pos ( sa.V[k].pos),
										  Rbuf, sa.comp            ) ;
		};

	};
};
//
///---------------------------------------------------------------------------
/// @struct tk_simple_move
/// @brief
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template <typename sort_alg>
struct tk_simple_move:public tk_mono<typename sort_alg::value_t >
{	//----------------- definitions ----------------------------
	typedef typename sort_alg::value_t 	value_t ;
	typedef range <value_t*>  			range_buf ;

	//------------------------------ variables -----------------------------
	sort_alg &sa ;
	itpos_t first, last ;

	tk_simple_move ( atomic_t *pf, sort_alg &s1, itpos_t f, itpos_t l):
		            tk_mono<value_t> ( pf),sa(s1),first (f), last (l){};

	void exec ( range_buf Rbuf)	{sa.move_simple_blocks ( first , last, Rbuf );};
};
//
//############################################################################
//                                                                          ##
//                                                                          ##
//                        T K _ D U A L                                     ##
//                                                                          ##
//                                                                          ##
//############################################################################
//
///---------------------------------------------------------------------------
/// @struct tk_dual
/// @brief base class for the not final elements
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template < typename value_t>
struct  tk_dual
{	//----------------- definitions & variables -----------------------------
	typedef range <value_t*>  	range_buf ;
	atomic_t 					*pfather, son ;
	uint32_t 					mode ;

	//------------------------- Functions -------------------------------------
	tk_dual ( atomic_t *pf, uint32_t as ): pfather(pf),son(as),mode(0){	};

	atomic_t  & get_son(void){ return son ;};

	virtual uint32_t exec ( range_buf RB  ) = 0;

	virtual ~tk_dual ()
	{	if ( pfather != nullptr ) bs_tools::atomic_sub<uint32_t>( *pfather, 1);
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
struct tk_split:public tk_dual<typename sort_alg::value_t >
{	//----------------- definitions ----------------------------
	typedef typename sort_alg::value_t 	value_t ;
	typedef range <value_t*>  			range_buf ;
	using tk_dual<value_t>::son ;
	using tk_dual<value_t>::mode ;

	//------------------ variables --------------------------
	sort_alg 	&sa ;
	range_pos	RP ;
	uint32_t 	Level ;

	//------------------------ functions -----------------------------------
	tk_split ( atomic_t *pf, sort_alg & s1 , range_pos R1 ,uint32_t Lv):
		     tk_dual<value_t> ( pf, 0),sa(s1),RP (R1), Level(Lv){};

	unsigned exec ( range_buf Rbuf );

};
template <class sort_alg>
unsigned  tk_split<sort_alg>::exec ( range_buf Rbuf )
{	//------------------------- begin ----------------------------
	if ( atomic_read ( son) != 0 ) return 2 ;
	switch ( mode )
	{	case 0 :
			sa.split_range ( &son, RP, Level);
			mode = 1 ;
			return 1 ;
		case 1 :
			mode = 2 ;
			sa.merge_range ( &son, RP,Rbuf);
			return 1 ;
	};
	return 0 ;
};
//
///---------------------------------------------------------------------------
/// @struct tk_global
/// @brief
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template < class sort_alg>
struct tk_global:public tk_dual<typename sort_alg::value_t >
{	//----------------- definitions & using -----------------------------
	typedef typename sort_alg::value_t 	value_t ;
	typedef range <value_t*>  			range_buf ;
	using tk_dual<value_t>::son ;
	using tk_dual<value_t>::mode ;

	//---------------------- variables ---------------------------------
	sort_alg &sa ;

	//---------------------- functions -----------------------------
	tk_global ( sort_alg & s1 ):tk_dual<value_t> (&s1.D.N,0), sa (s1){};

	unsigned exec ( range_buf  );

};
template < class sort_alg>
unsigned tk_global<sort_alg>::exec ( range_buf  )
{	//------------------------- begin ----------------------------
	typedef tk_split<sort_alg>	tk_split_t ;
	if ( atomic_read ( son) != 0 ) return 2 ;
	range_pos RP ( 0 , sa.NB ) ;
	switch ( mode )
	{	case 0 :
			mode = 1 ;
			atomic_write ( son , 1);
			sa.D.dq_dual.emplace_back ( new tk_split_t (&son, sa, RP,
					                                    sa.LevelNT -1));
			return 1 ;
		case 1 :
			mode = 2 ;
			sa.sort_blocks ( &son ) ;
			return 1 ;
	};
	return 0 ;
};

//
///---------------------------------------------------------------------------
/// @struct tk_move
/// @brief
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template <class sort_alg>
struct tk_move:public tk_dual<typename sort_alg::value_t >
{	//----------------- definitions ----------------------------
	typedef typename sort_alg::value_t 	value_t ;
	typedef range <value_t*>  			range_buf ;
	using tk_dual<value_t>::son ;
	using tk_dual<value_t>::mode ;

	//------------------------------ variables -------------------------------
	sort_alg & sa ;
	itpos_t first , last ;
	std::vector <size_t> Vseq ;

	//------------------------------ functions ------------------------------
	tk_move ( atomic_t *pf, sort_alg &s1 , itpos_t f, itpos_t l):
		      tk_dual<value_t> ( pf , 0 ), sa(s1),first(f),last(l){};

	uint32_t exec ( range_buf Rbuf);

};
template <class sort_alg>
uint32_t tk_move<sort_alg>::exec ( range_buf Rbuf)
{	//---------------- begin ------------------------
	if ( atomic_read ( son) != 0 ) return 2 ;
	switch ( mode)
	{	case 0 :
			mode = 1 ;
			return sa.move_blocks ( &son, first, last, Vseq, Rbuf );
		case 1:
			if ( Vseq.size() < 64 )
			{	sa.move_simple_blocks ( Vseq.begin() , Vseq.end(), Rbuf);
				return 0 ;
			}
			else
			{	atomic_write ( son, 1 );
				sa.D.dq_dual.emplace_back
				(new tk_move<sort_alg> (&son,sa,Vseq.begin(),Vseq.end()) );
				mode = 2 ;
				return 1 ;
			};
	};
	return 0 ;
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
using ptr_tkdual = std::unique_ptr <tk_dual<value_t> >  ;

template <typename value_t>
using dqcnc_tkmono = bs_tools::deque_cnc <ptr_tkmono<value_t> >   ;

template <typename value_t>
using dqcnc_tkdual = bs_tools::deque_cnc <ptr_tkdual<value_t> >   ;

template <typename value_t>
using dq_tkmono = std::deque <ptr_tkmono<value_t> >   ;

template <typename value_t>
using dq_tkdual = std::deque <ptr_tkdual<value_t> >   ;

//
///---------------------------------------------------------------------------
/// @struct
/// @brief
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template < typename value_t >
struct dispatcher
{	//--------------------- definitions --------------
	typedef range <value_t*>  	range_buf ;

	//------------------- variables ----------------------------
	dqcnc_tkmono <value_t>  dq_mono ;
	dqcnc_tkdual <value_t>  dq_dual ;
	atomic_t  N ;

	//--------------------- Functions -----------------------------------------
	dispatcher (uint32_t NN = 1):N(NN){};

	bool final ()
	{ 	return ( bs_tools::atomic_read<uint32_t>(N) == 0) ;
	};
	void exec (range_buf RB   );
};
template < typename value_t >
void  dispatcher<value_t>::exec (range_buf RB   )
{	//----------------------- begin -----------------------------------------
	ptr_tkmono<value_t> ptkm  ( nullptr) ;
	ptr_tkdual<value_t> ptkd  ( nullptr ) ;

	while ( not final())
	{	if ( dq_mono.pop_move_back( ptkm) )
		{	ptkm->exec(RB) ;
			delete (ptkm.release()) ;
		}
       	else
       	{	if (  dq_dual.pop_move_back( ptkd) )
			{	uint32_t code =ptkd->exec( RB) ;
				switch ( code)
				{	case 0: delete (ptkd.release()) ; break ;
					case 1: dq_dual.emplace_back(std::move(ptkd)); break ;
					case 2: dq_dual.emplace_front(std::move (ptkd));break ;
				};
			}
			else  std::this_thread::yield() ;
		};
	};
};
//
//****************************************************************************
};//    End namespace pbs
};//    End namespace algorithm
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
