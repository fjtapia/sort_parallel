//----------------------------------------------------------------------------
/// @file range.hpp
/// @brief
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_DETAIL_UTIL_RANGE_HPP
#define __BOOST_SORT_PARALLEL_DETAIL_UTIL_RANGE_HPP

#include <functional>
#include <memory>
#include <type_traits>
#include <iterator>
#include <vector>
#include <cassert>
#include <boost/sort/parallel/detail/util/low_level.hpp>

namespace boost		{
namespace sort		{
namespace parallel	{
namespace detail	{
namespace util		{

using std::iterator_traits ;


///---------------------------------------------------------------------------
/// @struct range
/// @brief this represent a range between two iterators
/// @tparam Iter_t type of paramenters of the range
/// @remarks
//----------------------------------------------------------------------------
template <class Iter_t>
struct range
{   //------------------------ variables -------------------
    Iter_t first , last ;

    //------------------------- functions ------------------
    range ( void){};
    range ( Iter_t frs, Iter_t lst):first ( frs),last (lst){};

    bool 	empty 		( void ) const { return (first == last );};
    bool 	not_empty	( void ) const { return (first != last );};
    bool   	valid 		( void ) const { return ((last-first)>= 0 ); };
    size_t 	size		( void ) const { return (last-first);};
    Iter_t  front       ( void ) const { return first ;};
    Iter_t  back        ( void ) const { return ( last -1);};
};

//
//-----------------------------------------------------------------------------
//  function : concat
/// @brief concatebate two contiguous ranges
/// @param [in] it1 : first range
/// @param [in] it2 : second range
/// @return  range resulting of the concatenation
//-----------------------------------------------------------------------------
template <class Iter_t>
range<Iter_t> concat ( const range<Iter_t> &it1 , const range <Iter_t> &it2 )
{	//--------------------------- begin -------------------------------------
	return range<Iter_t> ( it1.first , it2.last ) ;
};
//
//-----------------------------------------------------------------------------
//  function : move
/// @brief Move objets from the range src to dest
/// @param [in] dest : range where move the objects
/// @param [in] src : range from where move the objects
/// @return range with the objects moved and the size adjusted
//-----------------------------------------------------------------------------
template <class Iter1_t , class Iter2_t >
inline range<Iter2_t> init_move ( const range<Iter2_t> & dest,
		                          const range<Iter1_t> & src)
{   //------------- static checking ------------------------------------------
	typedef typename iterator_traits<Iter1_t>::value_type type1 ;
    typedef typename iterator_traits<Iter2_t>::value_type type2 ;
    static_assert ( std::is_same<type1, type2>::value,
                    "Incompatible iterators\n");

    //------------------------------- begin ----------------------------------
    if ( src.size() == 0 ) return range<Iter2_t>(dest.first, dest.first);
    init_move(dest.first ,src.first, src.last  );
    return range<Iter2_t>(dest.first, dest.first + src.size()) ;
};
//-----------------------------------------------------------------------------
//  function : uninit_move
/// @brief Move objets from the range src creatinf them in  dest
/// @param [in] dest : range where move and create the objects
/// @param [in] src : range from where move the objects
/// @return range with the objects moved and the size adjusted
//-----------------------------------------------------------------------------
template <class Iter1_t ,class Iter2_t >
inline range<Iter2_t> uninit_move ( const range<Iter2_t> &dest,
		                            const range<Iter1_t> &src  )
{   //------------- static checking ------------------------------------------
	typedef typename iterator_traits<Iter1_t>::value_type type1 ;
    typedef typename iterator_traits<Iter2_t>::value_type type2 ;
    static_assert ( std::is_same<type1, type2>::value,
                    "Incompatible iterators\n");

    //------------------------------- begin ----------------------------------
    if ( src.size() == 0 ) return range<Iter2_t>(dest.first, dest.first);
    uninit_move (dest.first,src.first, src.last  );
    return range<Iter2_t>(dest.first, dest.first + src.size()) ;
};
//
//-----------------------------------------------------------------------------
//  function : destroy
/// @brief destroy a range of objects
/// @param [in] r : range to destroy
//-----------------------------------------------------------------------------
template <class Iter_t >
inline void destroy ( range<Iter_t> r)
{   //----------------- begin ---------------------------
	destroy ( r.first, r.last);
};
//
//-----------------------------------------------------------------------------
//  function : init
/// @brief initialize a range of objects with the object val moving across them
/// @param [in] r : range of elements not initialized
/// @param [in] val : object used for the initialization
/// @return range initialized
//-----------------------------------------------------------------------------
template <class Iter_t >
inline range<Iter_t> init ( const range<Iter_t> & r,
				            typename iterator_traits< Iter_t>::value_type &val)
{   //----------------- begin ---------------------------
	init ( r.first, r.last , val);
	return r ;
};
//
//-----------------------------------------------------------------------------
//  function : is_mergeable
/// @brief : indicate if two ranges have a possible merge
/// @param [in] src1 : first range
/// @param [in] src2 : second range
/// @param [in] comp : object for to compare elements
/// @exception
/// @return true : they can be merged
///         false : they can't be merged
/// @remarks
//-----------------------------------------------------------------------------
template <class iter1_t  , class iter2_t , class compare >
inline bool is_mergeable (const range<iter1_t> &src1,
						  const range<iter2_t> &src2, compare  comp )
{   //---------------------------- begin ------------------------------------
	typedef typename iterator_traits<iter1_t>::value_type type1 ;
    typedef typename iterator_traits<iter2_t>::value_type type2 ;

    static_assert ( std::is_same<type1, type2>::value,
                   "Incompatible iterators\n");
    //---------------------------- begin --------------------------------------
    return comp ( *(src2.front()), *(src1.back()));
};

//
//-----------------------------------------------------------------------------
//  function : full_merge
/// @brief Merge two contiguous ranges src1 and src2 , and put the result in
///        the range dest, returning the range merged
/// @param [in] dest : range where locate the lements merged. the size of dest
///                    must be  greater or equal than the sum of the sizes of
///                    src1 and src2
/// @param [in] src1 : first range to merge
/// @param [in] src2 : second range to merge
/// @param [in] comp : comparison object
/// @return range with the elements merged and the size adjusted
//-----------------------------------------------------------------------------
template <class Iter1_t, class Iter2_t, class Iter3_t, class Compare >
inline range<Iter3_t> full_merge ( const range<Iter3_t> &dest,
                                   const range<Iter1_t> &src1,
                                   const range<Iter2_t> &src2, Compare  comp )
{   //------------------- metaprogramming ------------------------------------
	typedef typename iterator_traits<Iter1_t>::value_type type1 ;
    typedef typename iterator_traits<Iter2_t>::value_type type2 ;
    typedef typename iterator_traits<Iter3_t>::value_type type3 ;
    static_assert ( std::is_same<type1, type2>::value,
                   "Incompatible iterators\n");
    static_assert ( std::is_same<type3, type2>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    return range<Iter3_t> (dest.first,full_merge ( src1.first, src1.last,
    		               src2.first, src2.last , dest.first, comp) );
};

//-----------------------------------------------------------------------------
//  function : uninit_full_merge
/// @brief Merge two contiguous ranges src1 and src2 , and create and move the
///        result in the uninitialized range dest, returning the range merged
/// @param [in] dest : range where locate the elements merged. the size of dest
///                    must be  greater or equal than the sum of the sizes of
///                    src1 and src2. Initially is uninitialize memory
/// @param [in] src1 : first range to merge
/// @param [in] src2 : second range to merge
/// @param [in] comp : comparison object
/// @return range with the elements merged and the size adjusted
//-----------------------------------------------------------------------------
template <class Iter1_t, class Iter2_t, class value_t, class Compare >
inline range<value_t*> uninit_full_merge ( const range<value_t *> &dest,
                                           const range<Iter1_t> &src1,
                                           const range<Iter2_t> &src2,
										   Compare  comp                  )
{   //------------------- metaprogramming ------------------------------------
	typedef typename iterator_traits<Iter1_t>::value_type type1 ;
    typedef typename iterator_traits<Iter2_t>::value_type type2 ;
    static_assert ( std::is_same<type1, type2>::value,
                   "Incompatible iterators\n");
    static_assert ( std::is_same<value_t, type2>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    return range<value_t *> (dest.first,
    			             uninit_full_merge( src1.first, src1.last,
    			        		                src2.first, src2.last ,
												dest.first, comp        ));
};
//
//---------------------------------------------------------------------------
//  function : half_merge
/// @brief : Merge two buffers. The first buffer is in a separate memory
/// @param [in] dest : range where finish the two buffers merged
/// @param [in] src1 : first range to merge in a separate memory
/// @param [in] src2 : second range to merge, in the final part of the
///                    range where deposit the final results
/// @param [in] comp : object for compare two elements of the type pointed
///                    by the Iter1_t and Iter2_t
/// @return : range with the two buffers merged
//---------------------------------------------------------------------------
template <class Iter1_t, class Iter2_t, class Compare >
inline range<Iter2_t> half_merge ( 	const range<Iter2_t> &dest,
									const range<Iter1_t> &src1,
									const range<Iter2_t> &src2, Compare  comp )
{   //---------------------------- begin ------------------------------------
	typedef typename iterator_traits<Iter1_t>::value_type       type1 ;
	typedef typename iterator_traits<Iter2_t>::value_type       type2 ;
    static_assert ( std::is_same<type1, type2>::value,
                   "Incompatible iterators\n");

    //--------------------- code -------------------------------------------
    return range<Iter2_t>( dest.first ,
                           half_merge ( src1.first , src1.last,
                                        src2.first, src2.last,
										dest.first, comp         ) );
};
//
//-----------------------------------------------------------------------------
//  function : in_place_merge_uncontiguous
/// @brief : merge two non contiguous buffers src1 , src2, using the range
///          aux as auxiliary memory
/// @param [in] src1 : first range to merge
/// @param [in] src2 : second range to merge
/// @param [in] aux : auxiliary range used in the merge
/// @param [in] comp : object for to compare elements
/// @exception
/// @return true : not changes done
///         false : changes in the buffers
/// @remarks
//-----------------------------------------------------------------------------
template <class Iter1_t  , class Iter2_t , class Iter3_t, class Compare >
bool in_place_merge_uncontiguous ( const range<Iter1_t> &src1,
		                           const range<Iter2_t> &src2,
                                   const range<Iter3_t> &aux, Compare comp)
{	//------------------- metaprogramming ------------------------------------
	typedef typename iterator_traits<Iter1_t>::value_type type1 ;
    typedef typename iterator_traits<Iter2_t>::value_type type2 ;
    typedef typename iterator_traits<Iter3_t>::value_type type3 ;

    static_assert ( std::is_same<type1, type2>::value,
                   "Incompatible iterators\n");
    static_assert ( std::is_same<type3, type2>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
	return in_place_merge_uncontiguous ( src1.first, src1.last,
	                                     src2.first, src2.last,
	                                     aux.first, comp );
};
//
//-----------------------------------------------------------------------------
//  function : in_place_merge
/// @brief : merge two contiguous buffers ( src1, src2) using buf as
///          auxiliary memory
/// @param [in] src1 : first range to merge
/// @param [in] src1 : second range to merge
/// @param [in] buf : auxiliary memory used in the merge
/// @param [in] comp : object for to compare elements
/// @exception
/// @return true : not changes done
///         false : changes in the buffers
/// @remarks
//-----------------------------------------------------------------------------
template <class Iter1_t  , class Iter2_t , class Compare >
inline range<Iter1_t> in_place_merge (const range<Iter1_t> &src1,
									  const range<Iter1_t> &src2,
									  const range<Iter2_t> &buf, Compare  comp)
{   //---------------------------- begin ------------------------------------
	typedef typename iterator_traits<Iter1_t>::value_type type1 ;
    typedef typename iterator_traits<Iter2_t>::value_type type2 ;

    static_assert ( std::is_same<type1, type2>::value,
                   "Incompatible iterators\n");
    //---------------------------- begin --------------------------------------
	in_place_merge ( src1.first, src1.last, src2.last, buf.first, comp );
	return concat ( src1, src2);
};
//
//-----------------------------------------------------------------------------
//  function : merge_flow
/// @brief : merge two contiguous buffers
/// @tparam iter_t : iterator to the elements
/// @tparam compare : object for to compare two elements pointed by iter_t
///                   iterators
/// @param [in] first : iterator to the first element
/// @param [in] last : iterator to the element after the last in the range
/// @param [in] comp : object for to compare elements
/// @exception
/// @return true : not changes done
///         false : changes in the buffers
/// @remarks
//-----------------------------------------------------------------------------
template <class iter1_t  , class iter2_t , class compare >
inline void merge_flow (range<iter1_t> rng1,
		 	 	 	 	 range<iter2_t> rbuf,
						 range<iter1_t> rng2,  compare  cmp )
{   //---------------------------- begin ------------------------------------
	typedef typename iterator_traits<iter1_t>::value_type type1 ;
    typedef typename iterator_traits<iter2_t>::value_type type2 ;

    static_assert ( std::is_same<type1, type2>::value,
                   "Incompatible iterators\n");
    //---------------------------- begin --------------------------------------
 	range<iter2_t> rbx ( rbuf) ;
 	range<iter1_t> rx1(rng1), rx2( rng2) ;
 	//assert ( rbx.size() == rx1.size() and rx1.size() == rx2.size());
 	while ( rx1.first != rx1.last )
 	{   *(rx1.first++) = (cmp( *rbx.first,*rx2.first) ) ?
 								  std::move ( *(rbx.first++)):
 	                              std::move ( *(rx2.first++));

 	};

 	//if ( (rbx.first - rbuf.first ) != rx2.size())
 	//	std::cout <<"Problema \n\n\n";

 	if ( rx2.first == rx2.last ) return ;
 	if ( rbx.first == rbx.last) util::init_move ( rbuf, rng2);
 	else  	util::half_merge ( rbuf, rx2, rbx, cmp) ;
};
//****************************************************************************
};//    End namespace util
};//    End namespace detail
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
