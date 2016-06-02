//----------------------------------------------------------------------------
/// @file low_level.hpp
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
#ifndef __BOOST_SORT_PARALLEL_DETAIL_UTIL_LOW_LEVEL_HPP
#define __BOOST_SORT_PARALLEL_DETAIL_UTIL_LOW_LEVEL_HPP

#include <functional>
#include <memory>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <iterator>

namespace boost		{
namespace sort		{
namespace parallel	{
namespace detail    {
namespace util		{
//namespace lwl		{

using std::iterator_traits;

//
//-----------------------------------------------------------------------------
//  function : construct_object
/// @brief create an object in the memory specified by ptr
/// @tparam value_t : class of the object to create
/// @tparam Args : parameters for the constructor
/// @param [in] ptr : pointer to the memory where to create the object
/// @param [in] args : arguments to the constructor
//-----------------------------------------------------------------------------
template <class Value_t ,class ... Args>
inline void construct_object ( Value_t *ptr, Args && ... args)
{   (::new (static_cast<void*> (ptr)) Value_t (std::forward<Args> ( args)...));
};
//
//-----------------------------------------------------------------------------
//  function : destroy_object
/// @brief destroy an object in the memory specified by ptr
/// @tparam Value_t : class of the object to create
/// @param [in] ptr : pointer to the object to destroy
//-----------------------------------------------------------------------------
template <class Value_t >
inline void destroy_object ( Value_t *ptr){   ptr->~Value_t () ;};

//
//-----------------------------------------------------------------------------
//  function : init
/// @brief initialize a range of objects with the object val moving across them

/// @param [in] r : range of elements not initialized
/// @param [in] val : object used for the initialization
/// @return range initialized
//-----------------------------------------------------------------------------
template <class Iter_t >
inline void init ( Iter_t  first , Iter_t  last ,
				   typename iterator_traits< Iter_t>::value_type & val)
{   //----------------- begin ---------------------------
	if ( first == last  ) return  ;
	construct_object ( &(*first), std::move ( val) );
	Iter_t it1 = first, it2 = first+1 ;
	while ( it2 != last)
		construct_object (&(*( it2++)), std::move ( * (it1++)));
	val = std::move ( * (last -1));
};
//
//-----------------------------------------------------------------------------
//  function : construct
/// @brief create an object in the memory specified by ptr
/// @tparam Value_t : class of the object to create
/// @tparam Args : parameters for the constructor
/// @param [in] ptr : pointer to the memory where to create the object
/// @param [in] args : arguments to the constructor
//-----------------------------------------------------------------------------
template <class Value_t ,class ... Args>
inline void construct ( Value_t *ptr, Args && ... args)
{   (::new (static_cast<void*> (ptr)) Value_t (std::forward<Args> ( args)...));
};

//
//-----------------------------------------------------------------------------
//  function : init_move
/// @brief Move objets
/// @tparam Iter_t : iterator to the elements
/// @tparam value_t : class of the object to create
/// @param [in] itdest : iterator to the final place of the objects
/// @param [in] R : range to move
//-----------------------------------------------------------------------------
template <class Iter1_t , class Iter2_t >
inline Iter2_t init_move ( Iter2_t  it_dest, Iter1_t first,  Iter1_t last)
{   //----------------- begin ---------------------------
    while ( first != last )   *(it_dest++) = std::move (*(first++));
    return it_dest ;
};

//
//-----------------------------------------------------------------------------
//  function : uninit_move
/// @brief Move objets to uninitialized memory
/// @tparam Iter_t : iterator to the elements
/// @tparam value_t : class of the object to construct
/// @param [in] ptr : pointer to the memory where to create the object
/// @param [in] R : range to move
//-----------------------------------------------------------------------------
template < class Iter_t ,
           class Value_t = typename iterator_traits<Iter_t>::value_type >
inline Value_t * uninit_move ( Value_t * ptr, Iter_t  first,Iter_t  last  )
{   //----------------- begin ---------------------------
    typedef typename iterator_traits<Iter_t>::value_type value2_t ;
    static_assert ( std::is_same<Value_t, value2_t>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    while ( first != last )
        ::new (static_cast<void*> (ptr++)) Value_t (std::move ( *(first++)) );
    return ptr ;
};
//
//-----------------------------------------------------------------------------
//  function : destroy
/// @brief Move objets to uninitialized memory
/// @tparam Iter_t : iterator to the elements
/// @tparam value_t : class of the object to construct
/// @param [in] ptr : pointer to the memory where to construct the object
/// @param [in] R : range to move
//-----------------------------------------------------------------------------
template <class Iter_t >
inline void destroy ( Iter_t   first, const Iter_t  last )
{   //----------------- begin ---------------------------
    typedef typename iterator_traits<Iter_t>::value_type value_t ;
    while (first != last) (&(*(first++)))->~value_t () ;
};
//
//-----------------------------------------------------------------------------
//  function : full_merge
/// @brief Merge two contiguous buffers pointed by buf1 and buf2 , and put
///        in the buffer pointed by buf_out
/// @param [in] buf1 : iterator to the first element in the first buffer
/// @param [in] end_buf1 : final iterator of first buffer
/// @param [in] buf2 : iterator to the first iterator to the second buffer
/// @param [in] end_buf2 : final iterator of the second buffer
/// @param [in] buf_out : buffer where move the elements merged
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template <class Iter1_t, class Iter2_t, class Compare >
inline Iter2_t full_merge ( Iter1_t  buf1, const Iter1_t  end_buf1,
                            Iter1_t  buf2, const Iter1_t  end_buf2,
                            Iter2_t  buf_out, Compare  comp )
{   //------------------- metaprogramming ------------------------------------
    typedef typename iterator_traits<Iter1_t>::value_type value1_t ;
    typedef typename iterator_traits<Iter2_t>::value_type value2_t ;
    static_assert ( std::is_same<value1_t, value2_t>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    while ((buf1 != end_buf1) and (buf2 != end_buf2) )
    {   *(buf_out++) = (not comp(*buf2,*buf1))?  std::move(*(buf1++))
                                               : std::move (*(buf2++)) ;
    };
    return  ( buf1 == end_buf1)? init_move ( buf_out, buf2, end_buf2 )
                               : init_move ( buf_out, buf1, end_buf1 );
};
//
//-----------------------------------------------------------------------------
//  function : uninit_full_merge
/// @brief Merge two contiguous buffers pointed by first1 and first2 , and put
///        in the uninitialized buffer pointed by it_out
/// @param [in] first1 : iterator to the first element in the first buffer
/// @param [in] last : last iterator of the first buffer
/// @param [in] first2 : iterator to the first element to the second buffer
/// @param [in] last22 : final iterator of the second buffer
/// @param [in] it_out : uninitialized buffer where move the elements merged
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template <class Iter_t, class Value_t, class Compare >
inline Value_t* uninit_full_merge ( Iter_t  first1, const Iter_t  last1 ,
                                    Iter_t  first2, const Iter_t  last2 ,
                                    Value_t*   it_out, Compare comp )
{   //------------------------- metaprogramming -------------------------------
	typedef typename iterator_traits<Iter_t>::value_type type1 ;
    static_assert ( std::is_same<Value_t, type1>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    while ( first1 != last1 and first2 != last2 )
    {   construct ( (it_out++),(not comp(*first2,*first1))?
                             std::move(*(first1++)):std::move(*(first2++)));
    };
    return  ( first1 == last1 ) ? uninit_move ( it_out, first2, last2)
                                : uninit_move ( it_out, first1, last1);
};
//
//---------------------------------------------------------------------------
//  function : half_merge
/// @brief : Merge two buffers. The first buffer is in a separate memory. 
///          The second buffer have a empty space before buf2 of the same size 
///          than the (end_buf1 - buf1)
/// @param [in] buf1 : iterator to the first element of the first buffer
/// @param [in] end_buf1 : iterator to the last element of the first buffer
/// @param [in] buf2 : iterator to the first element of the second buffer
/// @param [in] end_buf2 : iterator to the last element of the second buffer
/// @param [in] buf_out : iterator to the first element to the buffer where put
///                       the result
/// @param [in] comp : object for Compare two elements of the type pointed
///                    by the Iter1_t and Iter2_t
/// @remarks The elements pointed by Iter1_t and Iter2_t must be the same
//---------------------------------------------------------------------------
template <class Iter1_t, class Iter2_t, class Compare >
inline Iter2_t half_merge ( Iter1_t  buf1 , const Iter1_t  end_buf1 ,
                            Iter2_t  buf2 , const Iter2_t  end_buf2 ,
                            Iter2_t  buf_out, Compare  comp     )
{   //---------------------------- begin ------------------------------------
    typedef typename iterator_traits<Iter1_t>::value_type value1_t ;
    typedef typename iterator_traits<Iter2_t>::value_type value2_t ;
    static_assert ( std::is_same<value1_t, value2_t>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    while ((buf1 != end_buf1) and (buf2 != end_buf2)  )
    {   *(buf_out++) = (not comp(*buf2,*buf1)) ? std::move (*(buf1++))
                                                :std::move (*(buf2++));
    };
    return ( buf2 == end_buf2)?init_move (buf_out , buf1, end_buf1 ):end_buf2 ;
};
//
//-----------------------------------------------------------------------------
//  function : in_place_merge_uncontiguous
/// @brief : merge two uncontiguous buffers, placing the results in the buffers
///          for to do this use an auxiliary buffer pointed by aux
/// @param [in] src1 : iterator to the first element of the first buffer
/// @param [in] end_src1 : last iterator  of the first buffer
/// @param [in] src2 : iterator to the first element of the second buffer
/// @param [in] end_src2 : last iterator  of the second buffer
/// @param [in] aux  : iterator to the first element of the auxiliary buffer
/// @param [in] comp : object for to Compare elements
/// @exception
/// @return true : not changes done
///         false : changes in the buffers
/// @remarks
//-----------------------------------------------------------------------------
template <class Iter1_t  , class Iter2_t , class Iter3_t, class Compare >
bool in_place_merge_uncontiguous ( Iter1_t  src1, const Iter1_t  end_src1 ,
                                   Iter2_t  src2, const Iter2_t  end_src2 ,
                                   Iter3_t  aux , Compare comp )
{	//------------------- metaprogramming ------------------------------------
	typedef typename iterator_traits<Iter1_t>::value_type type1 ;
	typedef typename iterator_traits<Iter2_t>::value_type type2 ;
	typedef typename iterator_traits<Iter3_t>::value_type type3 ;

    static_assert ( std::is_same<type1, type2>::value,
                   "Incompatible iterators\n");
    static_assert ( std::is_same<type3, type2>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    if ( src1 == end_src1 or src2 == end_src2 or
    	not comp ( *src2, *(end_src1 -1)) ) return true ;

    while ( src1 != end_src1 and not comp ( *src2, *src1)) ++src1;

	Iter3_t const  end_aux = aux + ( end_src1 - src1) ;
	Iter2_t  src2_first = src2 ;
    init_move (aux , src1 , end_src1);

	while ( ( src1 != end_src1) and (src2 != end_src2))
		*(src1++) = std::move((not comp (*src2,*aux))?*(aux++):*(src2++));

	if ( src2 == end_src2)
	{	while ( src1 != end_src1) *(src1 ++) = std::move ( *(aux ++));
		init_move (src2_first, aux , end_aux) ;
	}
	else
	{	half_merge ( aux, end_aux, src2, end_src2, src2_first ,comp);
	};
    return false ;
};

//
//-----------------------------------------------------------------------------
//  function : in_place_merge
/// @brief : merge two contiguous buffers,using an auxiliary buffer pointed
///          by buf
///
/// @param [in] src1: iterator to the first position of the first buffer
/// @param [in] src2: final iterator of the first buffer and first iterator
///                   of the second buffer
/// @param [in] end_src2 : final iterator of the second buffer
/// @param [in] buf  : iterator to buffer used as auxiliary memory
/// @param [in] comp : object for to Compare elements
/// @exception
/// @return true : not changes done
///         false : changes in the buffers
/// @remarks
//-----------------------------------------------------------------------------
template <class Iter1_t  , class Iter2_t , class Compare >
inline bool in_place_merge ( Iter1_t  src1, Iter1_t   src2 ,Iter1_t   end_src2,
                             Iter2_t  buf,  Compare  comp )
{   //---------------------------- begin ------------------------------------
	typedef typename iterator_traits<Iter1_t>::value_type 	type1 ;
    typedef typename iterator_traits<Iter2_t>::value_type 	type2 ;

    static_assert ( std::is_same<type1, type2>::value,
                   "Incompatible iterators\n");
    //---------------------------- begin --------------------------------------
    if ( src1 == src2 or src2 == end_src2 or
    	not comp (*src2 , * ( src2 -1)))	return true;

	Iter1_t end_src1 = src2 ;
	while ( src1 != end_src1 and not comp ( *src2, *src1)) ++src1 ;

	if ( src1 == end_src1 ) return false ;

	size_t nx  = end_src1 - src1 ;
	init_move (buf ,src1, end_src1 ) ;
	half_merge ( buf , buf + nx , src2, end_src2 ,src1,comp);
	return false ;
};
//****************************************************************************
//};//    End namespace lwl
};//    End namespace util
};//    End namespave detail
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
