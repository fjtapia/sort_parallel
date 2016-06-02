//----------------------------------------------------------------------------
/// @file   stack_cnc.hpp
/// @brief  This file contains the implementation concurrent stack
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_DETAIL_UTIL_stack_cnc_t_HPP
#define __BOOST_SORT_PARALLEL_DETAIL_UTIL_stack_cnc_t_HPP

#include <boost/sort/parallel/detail/util/spinlock.hpp>
#include <vector>

namespace boost		{
namespace sort		{
namespace parallel	{
namespace detail	{
namespace util		{
//
//###########################################################################
//                                                                         ##
//    ################################################################     ##
//    #                                                              #     ##
//    #                      C L A S S                               #     ##
//    #                   S T A C K _ C N C                          #     ##
//    #                                                              #     ##
//    ################################################################     ##
//                                                                         ##
//###########################################################################
//
//---------------------------------------------------------------------------
/// @class  stack_cnc
/// @brief This class is a concurrent stack controled by a spin_lock
/// @remarks
//---------------------------------------------------------------------------
template <typename T , typename Allocator=std::allocator<T> >
class stack_cnc
{
public:
//***************************************************************************
//                     D E F I N I T I O N S
//***************************************************************************
typedef std::vector<T,Allocator>                            vector_t ;
typedef typename vector_t::size_type                        size_type ;
typedef typename vector_t::difference_type                  difference_type ;
typedef typename vector_t::value_type                       value_type;
typedef typename vector_t::pointer                          pointer;
typedef typename vector_t::const_pointer                    const_pointer;
typedef typename vector_t::reference                        reference;
typedef typename vector_t::const_reference                  const_reference;
typedef typename vector_t::allocator_type                   allocator_type;
typedef Allocator                                           alloc_t ;



protected:
//---------------------------------------------------------------------------
//                   Internal variables
//---------------------------------------------------------------------------
vector_t            v_t ;
mutable spinlock_t  spl;

public :
//
//---------------------------------------------------------------------------
//  function : stack_cnc
/// @brief  constructor
//---------------------------------------------------------------------------
explicit inline stack_cnc (void ):v_t(){  } ;
//
//---------------------------------------------------------------------------
//  function : stack_cnc
/// @brief  constructor
/// @param [in] allc : Allocator
//---------------------------------------------------------------------------
explicit inline stack_cnc ( const Allocator &allc ):v_t(allc){  } ;
//
//---------------------------------------------------------------------------
//  function : stack_cnc
/// @brief  Copy constructor
//---------------------------------------------------------------------------
stack_cnc ( const stack_cnc &  ) = delete ;
//
//---------------------------------------------------------------------------
//  function : stack_cnc
/// @brief  Copy constructor
/// @param [in] v_other : stack_cnc from where copy the data
//---------------------------------------------------------------------------
template <typename Allocator2>
stack_cnc ( const std::vector<value_type,Allocator2> & v_other ):
            v_t (v_other) { };
//
//---------------------------------------------------------------------------
//  function : stack_cnc
/// @brief  Move constructor
//---------------------------------------------------------------------------
stack_cnc ( stack_cnc && ) = delete ;
//
//---------------------------------------------------------------------------
//  function : ~stack_cnc
/// @brief  Destructor
//---------------------------------------------------------------------------
virtual ~stack_cnc (void) {  v_t.clear(); };
//
//---------------------------------------------------------------------------
//  function : operator =
/// @brief Asignation operator
//---------------------------------------------------------------------------
stack_cnc & operator= (const stack_cnc &)  = delete ;
//
//---------------------------------------------------------------------------
//  function : operator =
/// @brief Asignation operator
/// @param [in] v_other : stack_cnc from where copy the data
/// @return Reference to the stack_cnc after the copy
//---------------------------------------------------------------------------
template < typename alloc_t2 >
stack_cnc & operator= (const std::vector<value_type,alloc_t2> &v_other)
{   //-------------------------- begin ------------------------------
    if ( this == &v_other ) return *this ;
    std::lock_guard<spinlock_t>  guard(spl);
    v_t = v_other ;
    return *this ;
};
//
//---------------------------------------------------------------------------
//  function : clear
/// @brief Delete all the elements of the stack_cnc.
//---------------------------------------------------------------------------
void clear(void)
{   std::lock_guard<spinlock_t>  guard(spl);
    v_t.clear ( );
};
//
//---------------------------------------------------------------------------
//  function : swap
/// @brief swap the data between the two stack_cnc
/// @param [in] A : stack_cnc to swap
/// @return none
//---------------------------------------------------------------------------
void swap ( stack_cnc  & stack_other ) noexcept
{   //------------------ begin ------------------
    if ( this == &stack_other ) return ;
    std::lock_guard<spinlock_t>  guard(spl);
    v_t.swap( stack_other.v_t);
};
//
//---------------------------------------------------------------------------
//  function : size
/// @brief return the number of elements in the stack_cnc
/// @return number of elements in the stack_cnc
//---------------------------------------------------------------------------
size_type size ( void) const noexcept
{   std::lock_guard<spinlock_t>  guard(spl);
    return v_t.size() ;
};
//
//---------------------------------------------------------------------------
//  function :max_size
/// @brief return the maximun size of the container
/// @return maximun size of the container
//---------------------------------------------------------------------------
size_type max_size (void) const noexcept
{   std::lock_guard<spinlock_t>  guard(spl);
    return ( v_t.max_size() );
};
//
//---------------------------------------------------------------------------
//  function : shrink_to_fit
/// @brief Requests the removal of unused capacity.
///        It is a non-binding request to reduce capacity() to size().
///        It depends on the implementation if the request is fulfilled.
///        All iterators, including the past the end iterator, are potentially
///        invalidated.
/// @return none
//---------------------------------------------------------------------------
void shrink_to_fit ( )
{   std::lock_guard<spinlock_t>  guard(spl);
    v_t.shrink_to_fit();
};
//
//---------------------------------------------------------------------------
//  function : capacity
/// @brief return the maximun size of the container
/// @return maximun size of the container
//---------------------------------------------------------------------------
size_type capacity ( void) const noexcept
{   std::lock_guard<spinlock_t>  guard(spl);
    return ( v_t.capacity() );
};
//
//---------------------------------------------------------------------------
//  function : empty
/// @brief indicate if the map is empty
/// @return true if the map is empty, false in any other case
//---------------------------------------------------------------------------
bool empty ( void) const noexcept
{   std::lock_guard<spinlock_t>  guard(spl);
    return (v_t.empty()) ;
};
//
//---------------------------------------------------------------------------
//  function : reserve
/// @brief Change the capacity for to contain , at least n elements
/// @param [in] n : number of elements for the new capacity
/// @return none
/// @remarks This function has not utility. It is provided only for
///          compatibility with the STL vector interface
//---------------------------------------------------------------------------
void reserve ( size_type n ) noexcept
{   std::lock_guard<spinlock_t>  guard(spl);
    v_t.reserve(n);
};
//
//---------------------------------------------------------------------------
//  function : copy
/// @brief explicit copy for to prevent the automatic copy with the operator =
/// @param [in] v_other : vector to copy
/// @return none
/// @remarks
//---------------------------------------------------------------------------
template< class Allocator2>
void copy ( std::vector<value_type,Allocator2> & v_other)
{   std::lock_guard<spinlock_t>  guard(spl);
    v_other = v_t ;
};

//---------------------------------------------------------------------------
//  function : push_back
/// @brief Insert one element in the back of the container
/// @param [in] val : value to insert. Can ve a value, a reference or an rvalue
/// @return iterator to the element inserted
/// @remarks This operation is O ( const )
//---------------------------------------------------------------------------
void push_back (const value_type  & val )
{   std::lock_guard<spinlock_t>  guard(spl);
    v_t.push_back(val);
};

//---------------------------------------------------------------------------
//  function : emplace_back
/// @brief Insert one element in the back of the container
/// @param [in] args :group of arguments for to build the object to insert
/// @return iterator to the element inserted
/// @remarks This operation is O ( const )
//---------------------------------------------------------------------------
template <class ... Args>
void emplace_back ( Args && ... args )
{   std::lock_guard<spinlock_t>  guard(spl);
    v_t.emplace_back (std::forward <Args>(args) ...);
};
//---------------------------------------------------------------------------
//  function : push_back
/// @brief Insert one element in the back of the container
/// @param [in] v_other : vector to insert : value to insert. Can ve a value,
///                       a reference or an rvalue
/// @return iterator to the element inserted
/// @remarks This operation is O ( const )
//---------------------------------------------------------------------------
template <class Allocator2>
stack_cnc & push_back ( const std::vector<value_type,Allocator2> & v_other)
{   std::lock_guard<spinlock_t>  guard(spl);
    for ( size_type i =0 ; i < v_other.size() ; ++i)  v_t.push_back(v_other[i]);
    return *this ;
};

//
//---------------------------------------------------------------------------
//  function :pop_back
/// @brief erase the last element of the container
/// @param [in] none
/// @return none
/// @remarks This operation is O(constant)
//---------------------------------------------------------------------------
void pop_back ( void)
{   std::lock_guard<spinlock_t>  guard(spl);
    v_t.pop_back() ;
};
//
//---------------------------------------------------------------------------
//  function :pop_copy_back
/// @brief erase the last element of the tree and return a copy
/// @param [out] val : reference to a variable where copy the element
/// @return code of the operation
///         0- Element erased
///         1 - Empty tree
/// @remarks This operation is O(1)
//---------------------------------------------------------------------------
bool pop_copy_back ( value_type & val)
{   //-------------------------- begin -----------------------------
    std::lock_guard<spinlock_t>  guard(spl);
    if ( v_t.size() == 0) return false ;
    val = v_t.back() ;
    v_t.pop_back() ;
    return true;
};
//
//---------------------------------------------------------------------------
//  function :pop_move_back
/// @brief erase the last element of the tree and return a copy
/// @param [out] V : reference to a variable where copy the element
/// @return code of the operation
///         0- Element erased
///         1 - Empty tree
/// @remarks This operation is O(1)
//---------------------------------------------------------------------------
bool pop_move_back ( value_type & P)
{   //-------------------------- begin -----------------------------
    std::lock_guard<spinlock_t>  S(spl);
    if ( v_t.size() == 0) return false ;
    P = std::move ( v_t.back()) ;
    v_t.pop_back() ;
    return true;
};
//
//---------------------------------------------------------------------------
//  function :pop_move_back
/// @brief erase the last NElem element of the stack, if possible, and push
///        back a copy in the vector V1
/// @param [in/out] V1 : vector where copy the elements extracted
/// @param [in] NElem : number of elements to extract, if possible
/// @return Number of elements extracted ( 0- Indicates that V is empty)
//---------------------------------------------------------------------------
template <class Allocator2>
size_type pop_move_back ( std::vector<value_type,Allocator2> & v1,
                          size_type nelem )
{   //-------------------------- begin -----------------------------
    std::lock_guard<spinlock_t>  s(spl);
    size_type aux = 0 ;
    if ( v_t.size() != 0 )
    {   aux = ( v_t.size() < nelem )? v_t.size() :nelem ;
        size_type pos_ini = v_t.size() - aux ;
        for ( size_type i = pos_ini ; i < v_t.size() ; ++i)
            v1.push_back ( std::move (v_t [i]));
        v_t.erase( v_t.begin() + pos_ini , v_t.end() );
    }
    return aux;
};
//
//---------------------------------------------------------------------------
//  function :pop_copy_back
/// @brief erase the last nelem element of the stack, if possible, and push
///        back a copy in the vector v_out
/// @param [in/out] v_out : vector where copy the elements extracted
/// @param [in] nelem : number of elements to extract, if possible
/// @return Number of elements extracted ( 0- Indicates that v_t is empty)
//---------------------------------------------------------------------------
template <class Allocator2>
size_type pop_copy_back ( std::vector<value_type,Allocator2> & v_out,
                          size_type nelem )
{   //-------------------------- begin -----------------------------
    std::lock_guard<spinlock_t>  guard(spl);
    size_type aux = 0 ;
    if ( v_t.size() != 0 )
    {   aux = ( v_t.size() < nelem )? v_t.size() :nelem ;
        size_type pos_ini = v_t.size() - aux ;
        for ( size_type i=pos_ini ;i< v_t.size();++i) v_out.push_back(v_t[i]);
        v_t.erase( v_t.begin() + pos_ini , v_t.end() );
    }
    return aux;
};

}; // end class stack_cnc

//***************************************************************************
};// end namespace util
};// end namespace detail
};// end namespace parallel
};// end namespace sort
};// end namespace boost
//***************************************************************************
#endif
