//----------------------------------------------------------------------------
/// @file block_indirect_sort.hpp
/// @brief Sample Sort algorithm
///
/// @author Copyright (c) 2016 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef  __BOOST_SORT_PARALLEL_DETAIL_BLOCK_INDIRECT_SORT_HPP
#define  __BOOST_SORT_PARALLEL_DETAIL_BLOCK_INDIRECT_SORT_HPP

#include <atomic>
#include <future>
#include <iterator>
#include <iostream>
#include <boost/sort/parallel/detail/util/nbits.hpp>
#include <boost/sort/parallel/detail/util/range.hpp>
#include <boost/sort/parallel/detail/intro_sort.hpp>
#include <boost/sort/parallel/detail/util/stack_cnc.hpp>
#include <boost/sort/parallel/detail/util/atomic.hpp>

namespace boost		{
namespace sort		{
namespace parallel	{
namespace detail	{


//
///---------------------------------------------------------------------------
/// @struct block_indirect_sort
/// @brief This a structure for to implement a sample sort, exception
///        safe
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
template <uint32_t block_size, uint32_t group_size, class iter_t, class compare>
struct block_indirect_sort
{
//----------------------------------------------------------------------------
//              F A S T _ B L O C K _ S O R T
//                  D E F I N I T I O N S
//----------------------------------------------------------------------------
typedef typename std::iterator_traits<iter_t>::value_type   value_t ;
typedef std::atomic<uint32_t> 				                atomic_t ;
typedef util::range <size_t>				                range_pos ;
typedef util::range <iter_t>                                range_it ;
typedef util::range <value_t*>                              range_buf ;
typedef std::function <void (void) >                        function_t ;

//
//---------------------------------------------------------------------------
/// @struct block_pos
/// @brief represent a group of N contiguous elements, beginning with the
///        pointed by first
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
class block_pos
{   //------------------------------ VARIABLES -----------------------------
	size_t num ;

public :
	//----------------------------- FUNCTIONS ------------------------------
    block_pos ( void): num(0){};
    block_pos (size_t position , bool side = false)
    {	num = (position << 1) + ((side)?1:0);
    };
    block_pos operator = ( size_t position )
    {   num = (position <<1) + (num&1);
        return *this ;
    };

    size_t pos (void) const{ return ( num>>1);};
    void   pos (size_t position ){ num = (position <<1) + (num&1);};

    bool side ( void) const { return ( (num &1) !=0 ) ;};
    void side ( bool b ){ num = ( num & ~1 ) + (b) ?1:0 ;};
//---------------------------------------------------------------------------
}; // end struct block_pos
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
/// @struct block
/// @brief represent a group of N contiguous elements, beginning with the
///        pointed by first
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
struct block
{   //------------------------------ VARIABLES ------------------------------
	iter_t first ;

	//-------------------------- FUNCTIONS --------------------------------
    block ( iter_t  it ):first(it){};

    iter_t 	 front     (void) const { return first ;};
    iter_t 	 back      (void) const { return first + block_size ;};
    range_it get_range (void)       { return range_it(first,first+block_size);};

//----------------------------------------------------------------------------
};// end struct block
//----------------------------------------------------------------------------
//
//-------------------------------------------------------------------------
//  function : compare_block
/// @brief compare two blocks using the content of the pointed by first
/// @param [in] b1 : first block to compare
/// @param [in] b2 : second block to compare
/// @param [in] cmp : comparison operator
//-------------------------------------------------------------------------
static bool compare_block ( block b1, block b2 , compare cmp = compare())
{   //---------------------------- begin ----------------------------------
    return cmp (*b1.first, *b2.first ) ;
};
//
///---------------------------------------------------------------------------
/// @struct compare_block_pos
/// @brief This is a object for to compare two blocks from their positions
///        in the index
//----------------------------------------------------------------------------
struct compare_block_pos
{   //-------------------------- VARIABLES --------------------------------
	iter_t first ;
    compare comp ;

    //-------------------------  FUNCTIONS -----------------------------------
    compare_block_pos ( iter_t f1, compare  cmp ):first (f1),comp(cmp){};

    bool operator() ( block_pos b1, block_pos b2 )const
    {   return comp ( *(first + (b1.pos() * block_size)),
    			      *(first + (b2.pos() * block_size))   ) ;
    };
//---------------------------------------------------------------------------
};// end struct compare_block_pos
//---------------------------------------------------------------------------
//
///---------------------------------------------------------------------------
/// @struct backbone
/// @brief This contains all the information shared betwen the classes of the
///        algorithm
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
struct backbone
{   //----------------------- VARIABLES --------------------------------------
    util::range<iter_t> global_range;
    std::vector<block_pos> index ;
    size_t nelem, nblock,ntail ;
    compare cmp ;
    range_it range_tail ;
    static thread_local value_t* buf;
    util::stack_cnc <std::function <void (void) > > works ;

    //------------------------------------------------------------------------
    //  function : backbone
    /// @brief constructor of the class
    /// @param [in] pos : position of the range
    /// @return iterator to the first position of the block
    //------------------------------------------------------------------------
    backbone ( iter_t first, iter_t last , compare comp):
               global_range ( first, last ), cmp(comp)
    {   //------------------------- begin ---------------------------------
        assert ( (last-first) >= 0 ) ;
        if ( first == last ) return ; //nothing to do

        nelem = size_t ( last - first ) ;
        nblock = (nelem + block_size -1 ) / block_size ;
        ntail = (nelem %block_size );
        index.reserve (nblock+1) ;

        for ( size_t i =0 ; i < nblock;++i)
        	index.emplace_back( block_pos(i));

        range_tail.first = (ntail ==0)?last:(first+ ((nblock-1) * block_size)) ;
        range_tail.last = last ;
    };

    //------------------------------------------------------------------------
    //  function : get_block
    /// @brief obtain the block in the position pos
    /// @param [in] pos : position of the range
    /// @return block required
    //------------------------------------------------------------------------
    inline block get_block ( size_t pos) const
    {	return block (global_range.first + ( pos * block_size ));
    };
    //-------------------------------------------------------------------------
    //  function : get_range
    /// @brief obtain the range in the position pos
    /// @param [in] pos : position of the range
    /// @return range required
    //-------------------------------------------------------------------------
    inline range_it get_range ( size_t pos) const
    {	//--------------------------- begin --------------------------------
    	iter_t it1 = global_range.first + ( pos * block_size );
        iter_t it2 = ( pos == (nblock-1) )?global_range.last:it1+block_size;
        return range_it ( it1 , it2);
    };
    //-------------------------------------------------------------------------
    //  function : get_range_buf
    /// @brief obtain the range in the position pos
    /// @param [in] pos : position of the range
    /// @return range
    //-------------------------------------------------------------------------
    inline range_buf get_range_buf ( ) const
    {	return range_buf ( buf, buf+ block_size) ;
    };
    //-------------------------------------------------------------------------
    //  function : exec
    /// @brief begin with the execution of the functions stored in works
    /// @param [in] counter : atomic counter. When 0 exits the function
    /// @return range
    //-------------------------------------------------------------------------
    void exec( atomic_t & counter )
    {   //------------------------ begin --------------------------------
    	std::function <void (void) >  func_exec ;
        while ( util::atomic_read (counter) != 0)
        {   if ( works.pop_move_back( func_exec) ) func_exec() ;
            else std::this_thread::yield() ;
		};
	};
    void exec( value_t * ptr_buf ,atomic_t & counter )
    {   //------------------------ begin --------------------------------
    	buf = ptr_buf ;
        exec ( counter) ;
	};
//---------------------------------------------------------------------------
};// end struct backbone
//---------------------------------------------------------------------------

//
///---------------------------------------------------------------------------
/// @struct parallel_sort
/// @brief This contains all the information shared betwen the classes of the
///        algorithm
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
struct parallel_sort
{   //----------------------- VARIABLES --------------------------------------
    backbone &bk ;
    size_t max_per_thread ;
    atomic_t counter ;

    //------------------------------------------------------------------------
    //  function : parallel_sort
    /// @brief constructor of the class
    /// @param [in] bkbn : backbone struct with all the information to sort
    /// @param [in] first : iterator to the first element to sort
    /// @param [in] last : iterator to the next element after the last
    //------------------------------------------------------------------------
    parallel_sort ( backbone &bkbn, iter_t first, iter_t last):
    	            bk(bkbn), counter(0)
    {   //-------------------------- begin ------------------------------------
        assert ( (last-first) >= 0 ) ;
        size_t nelem = size_t ( last - first ) ;

        //------------------- check if sort --------------------------------------
        bool sorted = true ;
        for ( iter_t it1 = first, it2 = first+1 ;
              it2 != last and (sorted = not bk.cmp(*it2,*it1));it1 = it2++);
        if (sorted) return ;

        //-------------------max_per_thread ---------------------------
        uint32_t nbits_size = (util::nbits64(sizeof (value_t)))>>1;
        if ( nbits_size > 5 ) nbits_size = 5 ;
        max_per_thread = 1<< (18 - nbits_size);

        //---------------- check if only single thread -----------------------
        if ( nelem < ( max_per_thread))
        {   intro_sort (first, last, bk.cmp);
            return ;
        };

        divide_sort ( first, last ) ;
        // wait until all the parts are finished
        bk.exec(counter);
    };
    //------------------------------------------------------------------------
    //  function : divide_sort
    /// @brief this function divide the data in two part, for to be sorted in
    ///        a parallel mode
    /// @param [in] first : iterator to the first element to sort
    /// @param [in] last : iterator to the next element after the last
    //------------------------------------------------------------------------
    inline void divide_sort(iter_t first, iter_t last   )
    {   //------------------- check if sort -----------------------------------
        bool sorted = true ;
        for ( iter_t it1 = first, it2 = first+1 ;
              it2 != last and (sorted = not bk.cmp(*it2,*it1));it1 = it2++);
        if (sorted) return ;

        //---------------- check if finish the subdivision -------------------
        size_t nelem  = last - first ;
        if ( nelem < ( max_per_thread))
        {   return intro_sort (first, last, bk.cmp);
        };

        //-------------------- pivoting  ----------------------------------
        pivot9 ( first, last ,bk.cmp);
        const value_t & val = const_cast < value_t &>(* first);
        iter_t c_first = first+1 , c_last  = last-1;

        while ( bk.cmp(*c_first, val) ) ++c_first ;
        while ( bk.cmp ( val,*c_last ) ) --c_last ;
        while (not( c_first > c_last ))
        {   std::swap ( *(c_first++), *(c_last--));
            while ( bk.cmp (*c_first, val) ) ++c_first;
            while ( bk.cmp ( val, *c_last) ) --c_last ;
        }; // End while
        std::swap ( *first , * c_last);

        // insert  the work of the second half in the stack of works
        util::atomic_add ( counter , 1 );
        function_t f1 = [=] ()
        		        { 	divide_sort ( c_first, last) ;
                            util::atomic_sub( this->counter,1);
                        };
        bk.works.emplace_back( f1) ;

        // The first half is done by the same thread
        divide_sort (first, c_last);
    };
//--------------------------------------------------------------------------
};// end struct parallel_sort
//--------------------------------------------------------------------------
//
///---------------------------------------------------------------------------
/// @struct merge_blocks
/// @brief This contains all the information shared betwen the classes of the
///        algorithm
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
struct merge_blocks
{   //---------------------- VARIABLES ------------------------------
    backbone &bk ;
    atomic_t counter ;
    //
    //-------------------------------------------------------------------------
    //  function : merge_blocks
    /// @brief make the indirect merge of the two range_pos inside RP
    /// @param [in] pf : pointer to the atomic counter of the father
    /// @param [in]	RP : rnage_pos which contains the two ranges to merge
    /// @param [in]	Rbuf : auxiliary memory for to do the merge
    //-------------------------------------------------------------------------
    merge_blocks ( backbone &bkb, size_t pos_index1, size_t pos_index2,
                   size_t pos_index3 ):bk(bkb),counter(0)
    {   //----------------------------- begin --------------------------------
        size_t nblock1 = pos_index2 - pos_index1 ;
        size_t nblock2 = pos_index3 - pos_index2 ;
        if ( nblock1 == 0 or nblock2 == 0 ) return ;

        //-----------------------------------------------------------------------
        // Merging of the two intervals
        //-----------------------------------------------------------------------
        std::vector<block_pos> vpos1, vpos2 ;
        vpos1.reserve ( nblock1 +1 );
        vpos2.reserve ( nblock2 +1 );

        for ( size_t i = pos_index1 ; i < pos_index2 ; ++i)
        {   vpos1.emplace_back( bk.index[i].pos(), true) ;
        };

        for ( size_t i = pos_index2; i < pos_index3 ; ++i)
        {   vpos2.emplace_back( bk.index[i].pos(), false) ;
        };
        //-------------------------------------------------------------------
        //  tail process
        //-------------------------------------------------------------------
        if ( vpos2.back().pos() == (bk.nblock -1) and
             bk.range_tail.first != bk.range_tail.last)
        {   tail_process ( vpos1, vpos2);
        	nblock1 = vpos1.size();
        	nblock2 = vpos2.size() ;
        };

        compare_block_pos  cmp_blk ( bk.global_range.first, bk.cmp) ;

        util::full_merge (vpos1.begin() , vpos1.end(),
                    vpos2.begin() , vpos2.end(),
                    bk.index.begin() + pos_index1, cmp_blk);

        //--------------------------------------------------------------------
        // Extracting the ranges for to merge the elements
        //--------------------------------------------------------------------
        std::vector <range_pos> vrange_pos ;
        cut_range (range_pos(pos_index1,pos_index1+nblock1+nblock2),vrange_pos);

        //-------------------------------------------------------------------
        //           parallel process
        //-------------------------------------------------------------------
        util::atomic_write ( counter , vrange_pos.size());
        for ( size_t i =0 ; i < vrange_pos.size() ; ++i)
        {	function_t f1=[=, &vrange_pos ]( )
        	              {	merge_range_pos ( vrange_pos[i]);
                            util::atomic_sub ( this->counter , 1);
        				  };
        	bk.works.emplace_back ( f1 );
        };
        bk.exec ( counter) ; // wait until finish all the ranges
    };
    //
    //-------------------------------------------------------------------------
    //  function : tail_process
    /// @brief make the indirect merge of the two range_pos inside RP
    /// @param [in] pf : pointer to the atomic counter of the father
    /// @param [in]	RP : rnage_pos which contains the two ranges to merge
    /// @param [in]	Rbuf : auxiliary memory for to do the merge
    //-------------------------------------------------------------------------
    void tail_process ( std::vector<block_pos> & vblkpos1,
                        std::vector<block_pos> & vblkpos2  )
    {   //-------------------------- begin ---------------------------------
    	if ( vblkpos1.size() == 0 or vblkpos2.size() == 0 ) return ;

        vblkpos2.pop_back();

        size_t posback1 = vblkpos1.back().pos() ;
        range_it range_back1 = bk.get_range (posback1 );

        if ( util::is_mergeable (range_back1,bk.range_tail, bk.cmp))
        { 	util::in_place_merge_uncontiguous ( range_back1, bk.range_tail,
                                                bk.get_range_buf(), bk.cmp);
        	if ( vblkpos1.size() >1 )
        	{   //---------------------------------------------------------------
        		size_t pos_aux = vblkpos1[ vblkpos1.size() -2].pos() ;
        		range_it range_aux = bk.get_range ( pos_aux) ;

        		if ( util::is_mergeable( range_aux,range_back1, bk.cmp))
        		{	vblkpos2.emplace_back( posback1, false ) ;
                	vblkpos1.pop_back() ;
        		};
        	};
        };
    };
    //
    //-------------------------------------------------------------------------
    //  function : cut_range
    /// @brief divide the range_pos rng in several ranges. The ranges obtained
    ///        are inserted in the vector of range_pos vrpos_out
    /// @param [in] rng : range to divide
    /// @param [in]	vrpos_out : vector where insert the ranges obtained in the
    ///                         division of rng
    //-------------------------------------------------------------------------
    inline void cut_range  (range_pos rng , std::vector <range_pos> & vrpos_out)
    {   //---------------------------- begin -----------------------------
        if ( rng.size() < group_size )
        {   vrpos_out.push_back ( rng);
            return ;
        };
        size_t npart = (rng.size() + group_size -1 )/ group_size ;
        size_t size_part = rng.size() / npart ;

        size_t pos_ini = rng.first ;
        size_t pos_last = rng.last ;
        while ( pos_ini < pos_last)
        {	size_t pos = pos_ini + size_part ;
            while ( pos < pos_last and
            		bk.index[pos-1].side() == bk.index[pos].side())
            {	++pos ;
            };
            if ( pos < pos_last)
            {	 in_place_merge_uncontiguous
									 ( bk.get_range ( bk.index[pos-1].pos()),
									   bk.get_range ( bk.index[pos].pos() ),
									   bk.get_range_buf(), bk.cmp);
            }
            else pos = pos_last ;
            if ( (pos - pos_ini) > 1 )
            	vrpos_out.emplace_back(  pos_ini, pos);
            pos_ini = pos ;
        };
    };
    //
    //-------------------------------------------------------------------------
    //  function : merge_range_pos
    /// @brief make the indirect merge of the two range_pos inside RP
    /// @param [in] rng : rango de positions of ranges for to merge
    //-------------------------------------------------------------------------
    inline void merge_range_pos ( range_pos rng )
     {   //------------------------- begin ------------------------------------
         if ( rng.size() < 2 ) return ;
         range_buf rbuf = bk.get_range_buf () ;

         range_it  rng_prev = bk.get_range ( bk.index[ rng.first ].pos());
         init_move ( rbuf, rng_prev);
         range_it rng_posx (rng_prev);

         for (size_t posx = rng.first +1 ; posx != rng.last ; ++posx )
         {	 rng_posx = bk.get_range ( bk.index[posx].pos());
         	 util::merge_flow ( rng_prev, rbuf, rng_posx, bk.cmp);
         	 rng_prev = rng_posx;

        }; // end while
        init_move (rng_posx, rbuf);
     }; // end  merge_range_pos ( range_pos rng )

//----------------------------------------------------------------------------
};// end struct merge_blocks
//----------------------------------------------------------------------------

//
///---------------------------------------------------------------------------
/// @struct move_blocks
/// @brief This contains all the information shared betwen the classes of the
///        algorithm
/// @tparam
/// @remarks
//----------------------------------------------------------------------------
struct move_blocks
{   //-------------------------- VARIABLES --------------------------------
    backbone &bk ;
    atomic_t counter ;
    //
    //-------------------------------------------------------------------------
    //  function : move_blocks
    /// @brief constructor of the class for to move the blocks to their true
    ///        position obtained from the index
    /// @param [in] bkb : backbone with the index and the blocks
    //-------------------------------------------------------------------------
    move_blocks ( backbone &bkb ):bk(bkb),counter(0)
    {   //------------------------- begin -------------------------------------
        std::vector < std::vector <size_t> > vsequence ;

        std::vector<size_t>  sequence ;
        size_t pos_index_ini =0 , pos_index_src =0, pos_index_dest =0 ;
        while ( pos_index_ini < bk.index.size() )
        {   while (pos_index_ini < bk.index.size() and
                   bk.index[pos_index_ini].pos() == pos_index_ini)
            {   ++pos_index_ini;
            };
            if ( pos_index_ini == bk.index.size() ) break ;

            sequence.clear() ;
            pos_index_src = pos_index_dest = pos_index_ini ;
            sequence.push_back (pos_index_ini);

            while ( bk.index[ pos_index_dest].pos() != pos_index_ini )
            {   pos_index_src = bk.index[ pos_index_dest].pos();
                sequence.push_back ( pos_index_src) ;

                bk.index[pos_index_dest].pos( pos_index_dest);
                pos_index_dest = pos_index_src ;
            };
            bk.index[ pos_index_dest].pos( pos_index_dest);
            vsequence.push_back( sequence);
        };
        util::atomic_write ( counter , vsequence.size() );

        //-------------------------------------------------------------------
        // creation of the works
        //-------------------------------------------------------------------
        atomic_t son_counter ( vsequence.size() ) ;
        for ( size_t i =0 ; i < vsequence.size() ; ++i )
        {   if ( vsequence[i].size() < group_size )
            {   function_t f1 = [ = , &vsequence, &son_counter]( )
        	                    {	move_sequence(vsequence[i] );
                                    util::atomic_sub ( son_counter, 1 ) ;
                                };
                bk.works.emplace_back ( f1 ) ;
            }
            else
            {   function_t f1 = [=, &vsequence, &son_counter]()
            	                {	move_long_sequence(vsequence[i]) ;
            						util::atomic_sub ( son_counter, 1 );
                                };
                bk.works.emplace_back ( f1 ) ;
            };
        };
        bk.exec ( son_counter) ;
    };
    //
    //-------------------------------------------------------------------------
    //  function : move_sequence
    /// @brief make the indirect merge of the two range_pos inside RP
    /// @param [in] pf : pointer to the atomic counter of the father
    /// @param [in]	RP : rnage_pos which contains the two ranges to merge
    /// @param [in]	Rbuf : auxiliary memory for to do the merge
    //-------------------------------------------------------------------------
    inline void move_sequence ( std::vector<size_t> & seq )
    {   //---------------------- begin -------------------------------------
        range_buf  rbuf = bk.get_range_buf () ;
        size_t pos_range2 = seq[0] ;

        range_it range2 = bk.get_range ( pos_range2);
        init_move ( rbuf, range2) ;

        for ( size_t i = 1 ; i < seq.size() ; ++i )
        {   pos_range2 = seq[i] ;
            range_it range1 ( range2) ;
            range2 = bk.get_range ( pos_range2 ) ;
            init_move ( range1, range2 ) ;
        };
        init_move ( range2, rbuf ) ;
    };
    //
    //-------------------------------------------------------------------------
    //  function : move_long_sequence
    /// @brief make the indirect merge of the two range_pos inside RP
    /// @param [in] pf : pointer to the atomic counter of the father
    /// @param [in]	RP : rnage_pos which contains the two ranges to merge
    /// @param [in]	Rbuf : auxiliary memory for to do the merge
    //-------------------------------------------------------------------------
    inline void move_long_sequence ( std::vector<size_t> & seq )
    {   //--------------------------------- begin ----------------------------
        if ( seq.size() < group_size )
            return move_sequence ( seq) ;

        size_t npart = ( seq.size() + group_size -1) / group_size ;
        size_t size_part = seq.size() / npart ;

        // vectors for the sequences and the final sequence
        std::vector< std::vector < size_t> > vseq ;
        vseq.reserve ( npart ) ;

        std::vector < size_t> index_seq ;
        index_seq.reserve ( npart ) ;

        auto it_pos = seq.begin() ;
        for ( size_t i =0 ; i < (npart -1) ; ++i, it_pos += size_part)
        {   vseq.emplace_back ( it_pos, it_pos + size_part) ;
            index_seq.emplace_back ( * ( it_pos+ size_part -1) ) ;
        };
        vseq.emplace_back ( it_pos , seq.end() ) ;
        index_seq.emplace_back ( seq.back() ) ;

        // creation of the functions
        atomic_t son_counter ( vseq.size() ) ;
        for ( size_t i =0 ; i < vseq.size() ; ++i )
        {   function_t f1=[=, &vseq, &son_counter]()
        	              {	move_sequence( vseq[i] ) ;
        	                util::atomic_sub ( son_counter, 1 );
                          };
            bk.works.emplace_back (f1 ) ;
        };
        bk.exec ( son_counter) ;
        move_long_sequence ( index_seq ) ;
    };
//---------------------------------------------------------------------------
};// end of struct move_blocks
//---------------------------------------------------------------------------
//
//------------------------------------------------------------------------
//                    FAST_BLOCK_SORT
//                VARIABLES AND CONSTANTS
//------------------------------------------------------------------------
backbone bk ;
atomic_t counter ;
value_t *ptr ;
bool construct ;
range_buf rglobal_buf ;
uint32_t nthread ;
//
//-------------------------------------------------------------------------
//  function : block_indirect_sort
/// @brief begin with the execution of the functions stored in works
/// @param [in] counter : atomic counter. When 0 exits the function
/// @return range
//-------------------------------------------------------------------------
block_indirect_sort ( iter_t first , iter_t last , compare cmp, uint32_t nthr )
                :bk (first, last, cmp), counter(0), ptr(nullptr),
				 construct (false ),nthread ( nthr)
{	//-------------------------- begin -------------------------------------
    assert ( (last-first) >= 0 ) ;
    size_t nelem = size_t ( last - first ) ;
    if ( nelem == 0 ) return ;

    //------------------- check if sort --------------------------------------
    bool sorted = true ;
    for ( iter_t it1 = first, it2 = first+1 ;
          it2 != last and (sorted = not bk.cmp(*it2,*it1));it1 = it2++);
    if (sorted) return ;

    //---------------- check if only single thread -----------------------
    uint32_t nbits_size = (util::nbits64(sizeof (value_t)))>>1;
    if ( nbits_size > 5 ) nbits_size = 5 ;
    size_t max_per_thread = 1<< (18 - nbits_size);


    if ( nelem < ( max_per_thread) or nthread < 2)
    {   intro_sort (first, last, bk.cmp);
        return ;
    };

    //----------------- creation of the temporary buffer --------------------
    ptr =std::get_temporary_buffer<value_t>( block_size*nthread).first ;
    if ( ptr == nullptr) throw std::bad_alloc() ;

    rglobal_buf = range_buf ( ptr , ptr + (block_size * nthread) );
    util::init ( rglobal_buf, *first);
    construct = true ;

    std::vector< value_t*> vbuf ( nthread);
    for ( uint32_t i =0 ; i < nthread ; ++i)
    	vbuf[i] = ptr + (i * block_size);

    //------------------------------------------------------------------------
	util::atomic_write ( counter , 1 ) ;
    function_t f1 = [&]()
    	            { 	 start_function( );
                         util::atomic_sub( counter , 1 ) ;
                    };
    bk.works.emplace_back ( f1 ) ;

    //------------------------------------------------------------------------
    //                    PROCESS
    //------------------------------------------------------------------------
    std::vector <std::future <void> > vfuture (nthread);

    for ( uint32_t i =0 ; i < nthread ; ++i)
    {	auto f1 = [=, &vbuf ] () {	bk.exec (vbuf[i], this->counter);};
    	vfuture[i] = std::async ( std::launch::async , f1 );
    };
    for ( uint32_t i =0 ; i < nthread ; ++i) vfuture[i].get() ;
};

block_indirect_sort ( iter_t first , iter_t last ):
block_indirect_sort ( first, last, compare(),
		              std::thread::hardware_concurrency() ) {};

block_indirect_sort (iter_t first , iter_t last , compare cmp):
block_indirect_sort ( first, last, cmp, std::thread::hardware_concurrency()){};

block_indirect_sort ( iter_t first , iter_t last , uint32_t nthread):
block_indirect_sort  ( first, last, compare(), nthread){};

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
    if (ptr != nullptr)
    {   if ( construct)
    	{   util::destroy (rglobal_buf);
        	construct = false ;
    	};
        std::return_temporary_buffer ( ptr) ;
    };
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
inline void split_range ( size_t pos_index1 ,size_t pos_index2,
		                  uint32_t level_thread )
{	//----------------------------- begin -----------------------------------
    size_t nblock = pos_index2 - pos_index1;

    //-------------------------------------------------------------------------
    // In the blocks not sorted, the physical position is the logical position
    //-------------------------------------------------------------------------
    iter_t first = bk.get_block ( pos_index1).first ;
    iter_t last = bk.get_range ( pos_index2 -1).last ;

    if ( nblock < group_size)
    {   intro_sort ( first, last , bk.cmp ) ;
        return ;
    };

    size_t pos_index_mid = pos_index1 + (nblock >>1 ) ;
    atomic_t son_counter (1) ;

    //-------------------------------------------------------------------------
    // Insert in the stack the work for the second part, and the actual thread,
    // execute the first part
    //-------------------------------------------------------------------------
    if ( level_thread != 0 )
    {   auto f1 = [=, &son_counter]( )
    	          {	split_range ( pos_index_mid,pos_index2, level_thread-1) ;
                    util::atomic_sub ( son_counter , 1 ) ;
                  };
        bk.works.emplace_back ( f1 ) ;
        split_range ( pos_index1 , pos_index_mid,level_thread -1) ;
    }
    else
    {   iter_t mid = first + ( (nblock >>1 ) * block_size ) ;
        auto f1 = [=, &son_counter]()
        		  {	parallel_sort ( bk, mid , last ) ;
                    util::atomic_sub ( son_counter , 1 ) ;
                  };
        bk.works.emplace_back ( f1 ) ;
        parallel_sort ( bk , first ,mid ) ;
    };
    bk.exec ( son_counter ) ;
    merge_blocks ( bk, pos_index1 , pos_index_mid , pos_index2 ) ;
};
//
//-----------------------------------------------------------------------------
//  function : start_function
/// @brief this function splits a range, and depending of the size, sort
///        directly or call to a recursive split_range
/// @param [in] pf : pointer to the atomic counter of the father
/// @param [in] RP : range of positions to split
/// @param [in]	Level : depth of the split
//-----------------------------------------------------------------------------
void start_function ( void)
{   //----------------------- begin -----------------------------------
	if (nthread < 9 )
	{	parallel_sort (bk, bk.global_range.first, bk.global_range.last);
	}
	else
	{	size_t level_thread = nbits64 ( nthread -1 );
    	split_range ( 0 , bk.nblock, level_thread -1) ;
    	move_blocks k( bk ) ;
	};
};
//----------------------------------------------------------------------------
};// End class block_indirect_sort
//----------------------------------------------------------------------------
template <uint32_t block_size,uint32_t group_size,class iter_t,class compare>
thread_local typename std::iterator_traits<iter_t>::value_type*
block_indirect_sort<block_size,group_size,iter_t,compare>::backbone::buf= nullptr ;
//
//****************************************************************************
};//    End namespace detail
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
