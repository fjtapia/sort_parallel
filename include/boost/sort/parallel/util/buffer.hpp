//----------------------------------------------------------------------------
/// @file buffer.hpp
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
#ifndef __BOOST_SORT_GENERAL_UTIL_BUFFER_HPP
#define __BOOST_SORT_GENERAL_UTIL_BUFFER_HPP

#include <functional>
#include <memory>
#include <type_traits>
#include <vector>
#include <boost/sort/parallel/util/util_iterator.hpp>
#include <boost/sort/parallel/util/algorithm.hpp>

namespace boost
{
namespace sort
{
namespace parallel
{
namespace util
{
//
//-----------------------------------------------------------------------------
//  function : construct
/// @brief create an object in the memory specified by Ptr
/// @tparam value_t : class of the object to create
/// @tparam Args : parameters for the constructor
/// @param [in] Ptr : pointer to the memory where to create the object
/// @param [in] args : arguments to the constructor
//-----------------------------------------------------------------------------
template <class value_t ,class ... Args>
inline void construct ( value_t *Ptr, Args && ... args)
{   (::new (static_cast<void*> (Ptr)) value_t (std::forward<Args> ( args)...));
};
//
//-----------------------------------------------------------------------------
//  function : move_buf
/// @brief Move objets
/// @tparam iter_t : iterator to the elements
/// @tparam value_t : class of the object to create
/// @param [in] itdest : iterator to the final place of the objects
/// @param [in] R : range to move
//-----------------------------------------------------------------------------
template <class iter1_t , class iter2_t >
inline iter2_t move_buf ( iter2_t it_dest, iter1_t first, iter1_t last)
{   //----------------- begin ---------------------------
    while ( first != last )   *(it_dest++) = std::move (*(first++));
    return it_dest ;
};
//
//-----------------------------------------------------------------------------
//  function : uninitialized_move
/// @brief Move objets to uninitialized memory
/// @tparam iter_t : iterator to the elements
/// @tparam value_t : class of the object to construct
/// @param [in] Ptr : pointer to the memory where to create the object
/// @param [in] R : range to move
//-----------------------------------------------------------------------------
template <class iter_t ,
          class value_t = typename iter_value<iter_t>::type >
inline value_t * uninit_move_buf ( value_t *Ptr, iter_t first, iter_t last )
{   //----------------- begin ---------------------------
    while ( first != last )
        ::new (static_cast<void*> (Ptr++)) value_t (std::move ( *(first++)) );
    return Ptr ;
};
//
//-----------------------------------------------------------------------------
//  function : destroy
/// @brief Move objets to uninitialized memory
/// @tparam iter_t : iterator to the elements
/// @tparam value_t : class of the object to construct
/// @param [in] Ptr : pointer to the memory where to construct the object
/// @param [in] R : range to move
//-----------------------------------------------------------------------------
template <class iter_t >
inline void destroy ( iter_t first, iter_t last )
{   //----------------- begin ---------------------------
    typedef typename iter_value<iter_t>::type value_t ;

    while (first != last) (&(*(first++)))->~value_t () ;
};
//
//-----------------------------------------------------------------------------
//  function : full_merge
/// @brief Merge two contiguous buffers pointed by first1 and first2 , and put
///        in the buffer pointed by P
/// @tparam iter1_t : iterator to the input buffers
/// @tparam iter2_t : iterator to the output buffers
/// @tparam compare : object to compate the elements pointed by iter1_t
/// @param [in] buf1 : iterator to the first element in the first buffer
/// @param [in] buf2 : iterator to the first iterator to the second buffer
/// @param [in] end_buf2 : final iterator of the second buffer
/// @param [in] buf_out : buffer where move the elements merged
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template <class iter1_t, class iter2_t, class compare >
inline iter2_t full_merge ( iter1_t buf1 , iter1_t end_buf1,
                            iter1_t buf2, iter1_t end_buf2,
                            iter2_t buf_out, compare  comp )
{   //------------------- metaprogramming ------------------------------------
    typedef typename iter_value<iter1_t>::type value1_t ;
    typedef typename iter_value<iter2_t>::type value2_t ;
    static_assert ( std::is_same<value1_t, value2_t>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    while ((buf1 != end_buf1) and (buf2 != end_buf2) )
    {   *(buf_out++) = (not comp(*buf2,*buf1))?  std::move(*(buf1++))
                                               : std::move (*(buf2++)) ;
    };
    return  ( buf1 == end_buf1)? move_buf ( buf_out, buf2, end_buf2 )
                               : move_buf ( buf_out, buf1, end_buf1 );
};
//
//-----------------------------------------------------------------------------
//  function : uninit_full_merge
/// @brief Merge two contiguous buffers pointed by first1 and first2 , and put
///        in the buffer pointed by P
/// @tparam iter1_t : iterator to the input buffers
/// @tparam iter2_t : iterator to the output buffers
/// @tparam compare : object to compate the elements pointed by iter1_t
/// @param [in] buf1 : iterator to the first element in the first buffer
/// @param [in] buf2 : iterator to the first iterator to the second buffer
/// @param [in] end_buf2 : final iterator of the second buffer
/// @param [in] buf_out : buffer where move the elements merged
/// @param [in] comp : comparison object
//-----------------------------------------------------------------------------
template <class iter_t, class value_t, class compare >
inline value_t* uninit_full_merge ( iter_t first1, iter_t last1 ,
                                    iter_t first2 ,iter_t last2 ,
                                    value_t* it_out, compare  comp )
{   //------------------------- metaprogramming -------------------------------
    typedef typename iter_value<iter_t>::type value1_t ;
    static_assert ( std::is_same<value1_t, value_t>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    while ( first1 != last1 and first2 != last2 )
    {   construct ( (it_out++),(not comp(*first2,*first1))?
                             std::move(*(first1++)):std::move(*(first2++)));
    };
    return  ( first1 == last1 ) ? uninit_move_buf ( it_out, first2, last2)
                                : uninit_move_buf ( it_out, first1, last1);
};
//
//---------------------------------------------------------------------------
//  function : half_merge
/// @brief : Merge two buffers. The first buffer is in a separate memory
/// @tparam iter1_t : iterator to the first buffer
/// @tparam iter2_t : iterator to the second buffer and the output buffer
/// @tparam compare : object to compate the elements pointed by the iterators
/// @param [in] buf1 : iterator to the first element of the first buffer
/// @param [in] end_buf1 : iterator to the last element of the first buffer
/// @param [in] buf2 : iterator to the first element of the second buffer
/// @param [in] end_buf2 : iterator to the last element of the second buffer
/// @param [in] buf_out : iterator to the first element to the buffer where put
///                       the result
/// @param [in] comp : object for compare two elements of the type pointed
///                    by the iter1_t and iter2_t
/// @exception
/// @return
/// @remarks
//---------------------------------------------------------------------------
template <class iter1_t, class iter2_t, class compare >
inline iter2_t half_merge ( iter1_t buf1   , iter1_t end_buf1 ,
                            iter2_t buf2   , iter2_t end_buf2 ,
                            iter2_t buf_out, compare  comp     )
{   //---------------------------- begin ------------------------------------
    typedef typename iter_value<iter1_t>::type value1_t ;
    typedef typename iter_value<iter2_t>::type value2_t ;
    static_assert ( std::is_same<value1_t, value2_t>::value,
                   "Incompatible iterators\n");
    //--------------------- code -------------------------------------------
    while ((buf1 != end_buf1) and (buf2 != end_buf2)  )
    {   *(buf_out++) = (not comp(*buf2,*buf1)) ? std::move (*(buf1++))
                                                :std::move (*(buf2++));
    };
    return ( buf2 == end_buf2)?move_buf (buf_out , buf1, end_buf1 ):buf_out ;
};
//****************************************************************************
};//    End namespace util
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
