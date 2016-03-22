//----------------------------------------------------------------------------
/// @file block_indirect_block.hpp
/// @brief Sample Sort algorithm
///
/// @author Copyright (c) 2010 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __SORT_ALGORITHM_BLOCK_INDIRECT_SORT_HPP
#define __SORT_ALGORITHM_BLOCK_INDIRECT_SORT_HPP

#include <atomic>
#include <future>
#include <iostream>
#include <iterator>
#include <boost/sort/parallel/tools/nbits.hpp>
#include <boost/sort/parallel/tools/nthread.hpp>
#include <boost/sort/parallel/util/range.hpp>
#include <boost/sort/parallel/algorithm/intro_sort.hpp>
#include <boost/sort/parallel/algorithm/tk_block_indirect_sort.hpp>

namespace boost		{
namespace sort		{
namespace parallel	{
namespace algorithm	{
namespace pbs		{
//
//----------------------------------------------------------------------------
//       U S I N G     S E N T E N C E S
//----------------------------------------------------------------------------
namespace bs_tools 	= boost::sort::parallel::tools ;
namespace bs_util 	= boost::sort::parallel::util ;
namespace bs_algo 	= boost::sort::parallel::algorithm;

using bs_tools::NThread ;
using std::iterator_traits ;
using bs_util::range ;
using bs_util::half_merge;
using bs_tools::NBits64 ;
using bs_algo::intro_sort ;
using bs_algo::deep::pivot9 ;

//----------------------------------------------------------------------------
//            D E F I N I T I O N S
//----------------------------------------------------------------------------
typedef std::atomic<uint32_t> 					atomic_t ;
typedef range <size_t>							range_pos ;

//
///---------------------------------------------------------------------------
/// @struct block
/// @brief represent a group of N contiguous elements, beginning with the
///        pointed by first
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
struct blk
{   size_t pos ;
	bool side ;
	blk ( size_t p=0 , bool s = false):pos(p), side (s) {};
};

//
///---------------------------------------------------------------------------
/// @struct compare_block
/// @brief This is a object for to compare two blocks comparing the first
///        element of the block_t
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template <unsigned BLK_SIZE, class iter_t, class compare>
struct compare_blk
{   //-------------- variables ------------------------
	iter_t first ;
    compare comp ;

    //---------------------- functions ---------------------------------------
    compare_blk ( iter_t f1, compare  cmp ):first (f1),comp(cmp){};
    bool operator() ( blk b1, blk b2 )const
    {   return comp ( *(first+(b1.pos*BLK_SIZE)),*(first+(b2.pos*BLK_SIZE))) ;
    };
};

//
///---------------------------------------------------------------------------
/// @struct block_indirect_sort
/// @brief This a structure for to implement a sample sort, exception
///        safe
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template < uint32_t BLK_SIZE,uint32_t RNG_SIZE, class iter_type,
           typename compare
           =std::less<typename iterator_traits<iter_type>::value_type>  >
struct block_indirect_sort
{
//----------------------------------------------------------------------------
//           DEFINITIONS AND CONSTANTS
//----------------------------------------------------------------------------
typedef iter_type                               iter_t;
typedef typename iterator_traits<iter_t>::value_type   	value_t ;
typedef value_t*                             	iterb_t ;
typedef range <iter_t>                      	range_it ;
typedef range <value_t*>                    	range_buf ;
typedef typename std::vector<size_t>::iterator  itpos_t ;
typedef block_indirect_sort<BLK_SIZE,RNG_SIZE,iter_t,compare> this_t ;

typedef tk_mono 		<value_t>			tk_mono_t ;
typedef tk_dual 		<value_t>			tk_dual_t ;
typedef tk_global 		<this_t>			tk_global_t ;
typedef tk_split 		<this_t>			tk_split_t ;
typedef tk_sort 		<this_t>			tk_sort_t ;
typedef tk_merge		<this_t>			tk_merge_t ;
typedef tk_simple_move	<this_t>			tk_simple_move_t ;
typedef tk_move 		<this_t>			tk_move_t ;

static const size_t Thread_min 	= BLK_SIZE * RNG_SIZE ;
//------------------------------------------------------------------------
//                VARIABLES AND CONSTANTS
//------------------------------------------------------------------------
std::vector<blk> V ;
std::vector <std::vector< size_t> >Vpos ;
dispatcher<value_t> D ;

range_it  R , Rtail;
range_buf Rglobal_buf ;

value_t *Ptr ;
compare comp ;
size_t NB ;
bool construct ;
uint32_t LevelNT ;

size_t MaxThread = 1<<16;
uint32_t LevelThread = 24 ;
const size_t MV_SIZE = RNG_SIZE ;
//
//-----------------------------------------------------------------------------
//  function : block_indirect_sort
/// @brief constructor of the class
/// @param [in] R1 : range of iterators to the elements to sot
/// @param [in] comp : object for to compare two elements
/// @param [in] NT : NThread object for to define the number of threads to use
///                  in the process. By default is the number of thread HW

//-----------------------------------------------------------------------------
block_indirect_sort ( range_it R1, compare cmp, NThread NT);
//
//-----------------------------------------------------------------------------
//  function :~block_indirect_sort
/// @brief destructor of the class. The utility is to destroy the temporary
///        buffer used in the sorting process
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
~block_indirect_sort ( void)
{   //----------------------------------- begin -------------------------
    if (Ptr != nullptr)
    {   if ( construct)
    	{   bs_util::destroy (Rglobal_buf);
        	construct = false ;
    	};
        std::return_temporary_buffer ( Ptr) ;
    };
};
//----------------------------------------------------------------------------
//  function : blk_pos
/// @brief obtain the block in the position pos
/// @param [in] pos : position of the range
/// @return iterator to the first position of the block
//----------------------------------------------------------------------------
inline iter_t blk_pos ( size_t p) const
{	return R.first + ( p * BLK_SIZE );
};
//----------------------------------------------------------------------------
//  function : rng_pos
/// @brief obtain the range in the position pos
/// @param [in] pos : position of the range
/// @return range
//----------------------------------------------------------------------------
inline range_it rng_pos ( size_t pos) const
{	iter_t it1 = blk_pos ( pos);
	iter_t it2 = ( pos == (NB-1) )?R.last : it1 + BLK_SIZE;
	return range_it ( it1 , it2);
};
//----------------------------------------------------------------------------
//  function : mergeable
/// @brief indicate if two ranges are independent or can be merged
/// @param [in] R1 : first range
/// @param [in] R2 : second range
/// @return false: are independent, true : can be merged
//----------------------------------------------------------------------------
template <class iter1_t , class iter2_t>
inline bool mergeable ( range<iter1_t> R1, range<iter2_t> R2)
{	return comp ( *R2.first , * (R1.last -1));
};

void split_range ( atomic_t *pf ,range_pos RP, uint32_t Level );

void merge_range (atomic_t *pf , range_pos RP , range_buf Rbuf);

void sort_blocks (atomic_t *pf);

void move_simple_blocks (itpos_t first, itpos_t last , range_buf Rbuf  );

uint32_t move_blocks (atomic_t *pf ,itpos_t first, itpos_t last,
		              std::vector<size_t> &Vseq , range_buf Rbuf );

//----------------------------------------------------------------------------
//  function : sort_thread
/// @brief this function is the work asigned to each thread in the parallel
///        process
/// @param [in] first : iterator to the first element to sort
/// @param [in] last : iterator to the next element after the last
/// @param [in] level : level of depth
//----------------------------------------------------------------------------
void sort_thread(atomic_t *pf,iter_t first, iter_t last , uint32_t level  );
//
//----------------------------------------------------------------------------
};//                    End class parallel_sort_in_place_tag
//----------------------------------------------------------------------------
//############################################################################
//                                                                          ##
//                                                                          ##
//           N O N   I N L I N E      F U N C T I O N S                     ##
//                                                                          ##
//                                                                          ##
//############################################################################
//
//-----------------------------------------------------------------------------
//  function : block_indirect_sort
/// @brief constructor of the class
/// @param [in] R1 : range of iterators to the elements to sot
/// @param [in] comp : object for to compare two elements
/// @param [in] NT : NThread object for to define the number of threads to use
///                  in the process. By default is the number of thread HW
//-----------------------------------------------------------------------------
template < uint32_t BLK_SIZE,uint32_t RNG_SIZE, class iter_type,class compare>
block_indirect_sort<BLK_SIZE, RNG_SIZE, iter_type,compare>::
block_indirect_sort ( range_it R1, compare cmp, NThread NT )
                     :R(R1),Rtail (R1), Ptr (nullptr) ,comp( cmp),
					  NB(0),construct (false)
{   //-------------------------- begin -------------------------------------
	assert ( R.valid() );
    size_t NElem = R.size() ;

    if ( NT() <2 or NElem <= ( Thread_min))
    {   intro_sort (R.first, R.last, comp);
        return ;
    };
	LevelNT = NBits64( NT()-1 ) ;

    //------------------- check if sort --------------------------------------
    bool SW = true ;
    for ( iter_t it1 = R.first, it2 = R.first+1 ;
          it2 != R.last and (SW = not comp(*it2,*it1));it1 = it2++);
    if (SW) return ;

    //-------------- MaxThread and LevelThread ----------------------
    uint32_t NBSize = (NBits64(sizeof (value_t)))>>1;
    if ( NBSize > 5 ) NBSize = 5 ;
    MaxThread = 1<< (18 - NBSize);
    LevelThread = (NBits64 ( MaxThread) <<1);

    //----------------- creation of the temporary buffer --------------------
    Ptr =std::get_temporary_buffer<value_t>( BLK_SIZE*NT()).first ;
    if ( Ptr == nullptr) throw std::bad_alloc() ;
    Rglobal_buf = range_buf ( Ptr , Ptr + (BLK_SIZE *NT()) );
    bs_util::init ( Rglobal_buf, *R.first);
    construct = true ;

    // --------------- create the ranges for the futures -------------------
    std::vector< range_buf>  VRB;
    VRB.reserve ( NT() ) ;
    value_t * P1 = Ptr ;
    for ( uint32_t i =0 ;i < NT() ; ++i, P1 += BLK_SIZE )
		VRB.emplace_back ( P1, P1 + BLK_SIZE ) ;

    // -------------------- create and fill Vpos ----------------------------
    NB = NElem / BLK_SIZE ;
    V.reserve ( NB +1 ) ;
    for ( size_t i =0 ; i <NB ; ++i) V.emplace_back ( i,true);
    Rtail.first = R.first + (NB * BLK_SIZE);
    if ( (NElem % BLK_SIZE) != 0 ) V.emplace_back ( NB, true);
    NB = V.size() ;

    //----------------------------------------------------------------------
    atomic_write ( D.N, 1);
    D.dq_dual.emplace_back ( new tk_global_t ( *this));

     //------------------------------------------------------------------------
    //                    PROCESS
    //------------------------------------------------------------------------
	std::vector <std::future <void> > F ( NT() ) ;

	for ( uint32_t i =0 ; i < NT() ; ++i)
		F[i] = std::async ( std::launch::async ,
				           &dispatcher<value_t>::exec, &D,VRB[i] );

	for ( uint32_t i =0 ; i < NT() ; ++i) F[i].get() ;
};
//
//-----------------------------------------------------------------------------
//  function : split_rage
/// @brief this function splits a range, and depending of the size, sort
///        directly or call to a recursive split_range
/// @param [in] pf : pointer to the atomic counter of the father
/// @param [in] RP : range of positions to split
/// @param [in]	Level : depth of the split
//-----------------------------------------------------------------------------
template < uint32_t BLK_SIZE, uint32_t RNG_SIZE,
           class iter_type, typename compare     >
void block_indirect_sort<BLK_SIZE, RNG_SIZE, iter_type,compare>::
split_range ( atomic_t *pf ,range_pos RP, uint32_t Level )
{	//----------------------------- begin -----------------------------------
	size_t NB1 = (RP.size() )>>1 ;
	range_pos RP1 ( RP.first,RP.first + NB1 );
	range_pos RP2 ( RP.first + NB1 , RP.last);
	bs_tools::atomic_write ( *pf, 2);
	if ( Level == 0 )
	{	iter_t it1 = blk_pos ( RP1.first);
		iter_t it2 = it1 + (NB1 * BLK_SIZE) ;
		iter_t it3 = rng_pos ( RP2.last-1).last;
		D.dq_mono.emplace_back ( new tk_sort_t ( pf,*this, it1, it2, Level -1));
		D.dq_mono.emplace_back ( new tk_sort_t ( pf,*this, it2, it3, Level -1));
	}
	else
	{	D.dq_dual.emplace_back (new tk_split_t ( pf, *this, RP1, Level -1) );
		D.dq_dual.emplace_back (new tk_split_t ( pf, *this, RP2, Level -1) );
	};
};
//
//-----------------------------------------------------------------------------
//  function : merge_range
/// @brief make the indirect merge of the two range_pos inside RP
/// @param [in] pf : pointer to the atomic counter of the father
/// @param [in]	RP : rnage_pos which contains the two ranges to merge
/// @param [in]	Rbuf : auxiliary memory for to do the merge
//-----------------------------------------------------------------------------
template < uint32_t BLK_SIZE, uint32_t RNG_SIZE,
           class iter_type, typename compare     >
void block_indirect_sort<BLK_SIZE, RNG_SIZE, iter_type,compare>::
merge_range (atomic_t *pf , range_pos RP , range_buf Rbuf)
{	//------------------------ begin ------------------------------
	size_t NB1 = (RP.size() )>>1 ;
	range_pos RP1 ( RP.first,RP.first + NB1 );
	range_pos RP2 ( RP.first + NB1 , RP.last);

	//-----------------------------------------------------------------------
	// Merging of the two intervals
	//-----------------------------------------------------------------------
	std::vector<blk> Vb1, Vb2 ;
	Vb1.reserve ( RP1.size() );
	Vb2.reserve ( RP2.size() );

	for ( size_t i = RP1.first ; i < RP1.last ; ++i)
		Vb1.emplace_back( V[i].pos, true) ;
	for ( size_t i = RP2.first; i < RP2.last ; ++i)
		Vb2.emplace_back( V[i].pos, false) ;

	//  tail process
	bool Btail = ( Vb2.back().pos == (NB -1) and Rtail.first != Rtail.last);
	if ( Btail)
	{	Vb2.pop_back();
		size_t posback = V[RP1.last - 1].pos ;
		range_it rback = rng_pos (posback );
		if ( not in_place_merge_uncontiguous ( rback, Rtail, Rbuf, comp) )
		{	iter_t itaux = blk_pos(V[RP1.last -2].pos )+ (BLK_SIZE -1);
			if (  comp ( *rback.first , *itaux) )
			{	Vb2.emplace_back( Vb1.back());
				Vb1.pop_back();
			};
		};
	};

	compare_blk<BLK_SIZE, iter_t ,compare>  cmp_blk ( R.first, comp) ;

	bs_util::lwl::full_merge (Vb1.begin() , Vb1.end(),
			                     Vb2.begin() , Vb2.end(),
								 V.begin() + RP.first, cmp_blk);

	//------------------------------------------------------------------------
	// Extracting the ranges for to merge the elements
	//------------------------------------------------------------------------
	std::vector <range_pos> Vrp ;
	Vrp.reserve ( RP.size() / RNG_SIZE +1);
	if ( RNG_SIZE >= RP.size() )
	{	Vrp.emplace_back ( RP);
		return ;
	};
	size_t PosIni = RP.first ;
	size_t PosLast = (Btail)? RP.last -1:RP.last ;
	while ( PosIni < PosLast)
	{	size_t Pos = PosIni + RNG_SIZE ;
		while ( Pos < PosLast and  V[Pos-1].side == V[Pos].side) ++Pos ;
		if ( Pos < PosLast)
			in_place_merge_uncontiguous ( rng_pos ( V[Pos-1].pos),
					                      rng_pos (V[Pos].pos), Rbuf, comp);
		else Pos = PosLast ;
		Vrp.emplace_back(  PosIni, Pos);
		PosIni = Pos ;
	};

	bs_tools::atomic_write ( *pf, Vrp.size());
	for ( size_t i = 0 ; i < Vrp.size(); ++i)
	{	D.dq_mono.emplace_back(new tk_merge_t ( pf,*this, Vrp[i]));
	};
};
//
//-----------------------------------------------------------------------------
//  function : sort_blocks
/// @brief generate the vectors with the movements , needed for to move the
///        elements according with the index V
/// @param [in] pf : pointer to the atomic counter of the father
//-----------------------------------------------------------------------------
template < uint32_t BLK_SIZE, uint32_t RNG_SIZE,
           class iter_type, typename compare     >
void block_indirect_sort<BLK_SIZE, RNG_SIZE, iter_type,compare>::
sort_blocks (atomic_t *pf)
{	//------------------------------------------------------------------------
	//        sort blocks
	//------------------------------------------------------------------------
	std::vector<size_t>  Vaux ;
	size_t pos_ini =0 , pos_src =0, pos_dest =0 ;
	while ( pos_ini < V.size() )
	{   while (pos_ini < V.size() and V[pos_ini].pos == pos_ini) ++pos_ini;
		if ( pos_ini == V.size() ) break ;
		Vaux.clear() ;
		pos_src = pos_dest = pos_ini ;
		Vaux.push_back (pos_ini);
		while ( V[ pos_dest].pos != pos_ini )
		{   pos_src = V[ pos_dest].pos;
			Vaux.push_back ( pos_src) ;
			V[ pos_dest].pos = pos_dest;
			pos_dest = pos_src ;
		};
		V[ pos_dest].pos = pos_dest;
		Vpos.push_back( Vaux);
	};
	bs_tools::atomic_write ( *pf, Vpos.size() );
	dq_tkmono <value_t> dqm ;
	dq_tkdual <value_t> dqd ;
	for ( size_t i =0 ; i < Vpos.size() ; ++i)
	{	if ( Vpos[i].size() < MV_SIZE )
			dqm.emplace_back( new tk_simple_move_t ( pf,*this, Vpos[i].begin(),
					                                 Vpos[i].end() ));
		else
			dqd.emplace_back( new tk_move_t  ( pf ,*this , Vpos[i].begin() ,
					                           Vpos[i].end()));
	};
	if ( dqm.size() != 0 ) D.dq_mono.push_back (std::move (dqm));
	if ( dqd.size() != 0 ) D.dq_dual.push_back (std::move( dqd));

};
//
//-----------------------------------------------------------------------------
//  function : move_simple_blocks
/// @brief move the blocks, which positions are between the iterators first
///        and last
/// @param [in] first : itertor to the first element to move
/// @param [in] last : iterator to the last  element to move
/// @param [in] Rbuf : auxiliary memory for to to move the ranges
//-----------------------------------------------------------------------------
template < uint32_t BLK_SIZE, uint32_t RNG_SIZE,
           class iter_type, typename compare     >
void block_indirect_sort<BLK_SIZE, RNG_SIZE, iter_type,compare>::
move_simple_blocks (itpos_t first, itpos_t last , range_buf Rbuf  )
{	//----------------------- begin ----------------------------------------
	if ( first == last ) return ;
	range_buf RB = init_move ( Rbuf , rng_pos ( *first));
	for ( itpos_t it1 = first +1 ; it1 != last ; ++it1)
		init_move (rng_pos ( *(it1-1)), rng_pos ( *it1) );
	init_move ( rng_pos ( *(last-1)), RB);
};
//
//-----------------------------------------------------------------------------
//  function : move_blocks
/// @brief split  the long movement sequence in parts and create objects
///        tk_simple_move_t  and insert in the deque
/// @param [in] pf : pointer to the atomic counter of the father
/// @param [in] first : first position of the block to move
/// @param [in] last : last position of the block to move
/// @param [in] Vseq : When the sequence is very long, is divided in parts,
///                    and appear a new sequence , which must be done when all
///                    the others are finished. This sequence is Vseq
/// @param [in] Rbuf : auxiliary memory for to do the movements
/// @return 0 : all is done, Vseq empty 1 : long sequence
/// @remarks
//-----------------------------------------------------------------------------
template < uint32_t BLK_SIZE, uint32_t RNG_SIZE,
           class iter_type, typename compare     >
uint32_t block_indirect_sort<BLK_SIZE, RNG_SIZE, iter_type,compare>::
move_blocks ( atomic_t *pf ,itpos_t first, itpos_t last,
		      std::vector<size_t> &Vseq , range_buf Rbuf )
{	//----------------------- begin ----------------------------------------
	Vseq.clear() ;
	bs_tools::atomic_write ( *pf, 0);
	if ( first == last ) return 0;
	size_t NElem = last - first ;
	if ( NElem <= MV_SIZE )
	{	move_simple_blocks( first, last,Rbuf) ;
		return 0 ;
	};
	size_t NB = NElem / MV_SIZE ;
	if ( (NElem % MV_SIZE )!= 0 ) NB++ ;

	//std::vector<size_t> Vseq ;
	Vseq.reserve ( NB);
	itpos_t it1 = first ;
	bs_tools::atomic_write ( *pf, NB);
	for ( size_t i =0 ; i < NB ; ++i)
	{	itpos_t it2 = ( i == (NB-1))? last:it1+MV_SIZE ;
		Vseq.push_back (*(it2-1) );
		D.dq_mono.emplace_back( new tk_simple_move_t ( pf, *this, it1, it2));
		it1 = it2 ;
	};
	return 1 ;
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
template < uint32_t BLK_SIZE, uint32_t RNG_SIZE,
           class iter_type, typename compare     >
void block_indirect_sort<BLK_SIZE, RNG_SIZE, iter_type,compare>::
sort_thread ( atomic_t *pf, iter_t  first,
              iter_t   last , uint32_t level  )
{   //------------------------- begin ----------------------
 	size_t N  = last - first ;

 	if ( N <= MaxThread or level < LevelThread)
    {   bs_algo::deep::intro_sort_internal ( first, last, level,comp) ;
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
    bs_tools::atomic_add ( *pf ,1);

    D.dq_mono.emplace_back (new tk_sort_t(pf,*this ,c_first, last, level-1));
	sort_thread (pf,first, c_last, level-1);
    return ;
};


//****************************************************************************
};//    End namespace pbs
//****************************************************************************
namespace bs_tools 	= boost::sort::parallel::tools ;
namespace bs_util 	= boost::sort::parallel::util ;
namespace bs_algo 	= boost::sort::parallel::algorithm;

using bs_tools::NThread ;
using std::iterator_traits ;
using bs_util::range ;

//
//############################################################################
//                                                                          ##
//                 F U N C T I O N   F O R M A T                            ##
//                             A N D                                        ##
//               I N D I R E C T   F U N C T I O N S                        ##
//                                                                          ##
// These functions are for to select the correct format depending of the    ##
// number and type of the parameters                                        ##
//############################################################################
//
//-----------------------------------------------------------------------------
//  function : block_indirect_sort
/// @brief invocation to the parallel block_indirect_sort algorithm
/// @param [in] firts : iterator to the first element of the range to sort
/// @param [in] last : iterator after the last element to the range to sort
/// @param [in] NT : This object is a integer from the ranges [1, UINT32_MAX].
///                  by default is the number of HW threads of the machine
//-----------------------------------------------------------------------------
template < class iter_t >
void block_indirect_sort ( iter_t first, iter_t last ,
		                             NThread NT = NThread() )
{   //----------------------------------- begin ------------------------------
	typedef typename iterator_traits<iter_t>::value_type 	value_t ;
    typedef std::less<value_t > 				compare;
    typedef pbs::block_indirect_sort <1024,64,iter_t,compare> para_sort ;
	bs_util::range<iter_t> R ( first, last ) ;
	assert ( R.valid() ) ;
    para_sort PS( R, compare(),NT);
};
//
//-----------------------------------------------------------------------------
//  function : block_indirect_sort
/// @brief invocation to the parallel block_indirect_sort algorithm
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
void block_indirect_sort ( iter_t first, iter_t last,
                           compare comp, NThread NT = NThread() )
{   //----------------------------- begin ----------------------------------
    typedef pbs::block_indirect_sort <1024,64,iter_t,compare> para_sort ;
	bs_util::range<iter_t> R ( first, last ) ;
	assert ( R.valid() ) ;
    para_sort PS( R, comp,NT);
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
