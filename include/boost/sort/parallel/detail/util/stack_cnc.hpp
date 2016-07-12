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

namespace boost    {
namespace sort     {
namespace parallel {
namespace detail   {
namespace util     {
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
template <typename T, typename Allocator = std::allocator<T>>
class stack_cnc
{
  public:
    //***************************************************************************
    //                     D E F I N I T I O N S
    //***************************************************************************
    typedef std::vector<T, Allocator> vector_t;
    typedef typename vector_t::size_type size_type;
    typedef typename vector_t::difference_type difference_type;
    typedef typename vector_t::value_type value_type;
    typedef typename vector_t::pointer pointer;
    typedef typename vector_t::const_pointer const_pointer;
    typedef typename vector_t::reference reference;
    typedef typename vector_t::const_reference const_reference;
    typedef typename vector_t::allocator_type allocator_type;
    typedef Allocator alloc_t;

  protected:
    //-------------------------------------------------------------------------
    //                   Internal variables
    //-------------------------------------------------------------------------
    vector_t v_t;
    mutable spinlock_t spl;

  public:
    //
    //-------------------------------------------------------------------------
    //  function : stack_cnc
    /// @brief  constructor
    //-------------------------------------------------------------------------
    explicit stack_cnc(void) : v_t(){};
    //
    //-------------------------------------------------------------------------
    //  function : stack_cnc
    /// @brief  constructor
    /// @param allc : Allocator
    //-------------------------------------------------------------------------
    explicit stack_cnc(const Allocator &allc) : v_t(allc){};
    //
    //-------------------------------------------------------------------------
    //  function : stack_cnc
    /// @brief  Copy constructor
    //-------------------------------------------------------------------------
    stack_cnc(const stack_cnc &) = delete;
    //
    //-------------------------------------------------------------------------
    //  function : stack_cnc
    /// @brief  Copy constructor
    /// @param v_other : stack_cnc from where copy the data
    //-------------------------------------------------------------------------
    template <typename Allocator2>
    stack_cnc(const std::vector<value_type, Allocator2> &v_other)
        : v_t(v_other){};
    //
    //-------------------------------------------------------------------------
    //  function : stack_cnc
    /// @brief  Move constructor
    //-------------------------------------------------------------------------
    stack_cnc(stack_cnc &&) = delete;
    //
    //-------------------------------------------------------------------------
    //  function : ~stack_cnc
    /// @brief  Destructor
    //-------------------------------------------------------------------------
    virtual ~stack_cnc(void) { v_t.clear(); };
    //
    //-------------------------------------------------------------------------
    //  function : operator =
    /// @brief Asignation operator
    //-------------------------------------------------------------------------
    stack_cnc &operator=(const stack_cnc &) = delete;
    //
    //-------------------------------------------------------------------------
    //  function : operator =
    /// @brief Asignation operator
    /// @param v_other : vector from where copy the data
    /// @return Reference to the stack_cnc after the copy
    //-------------------------------------------------------------------------
    template <typename alloc_t2>
    stack_cnc &operator=(const std::vector<value_type, alloc_t2> &v_other)
    {   
        if (this == &v_other) return *this;
        std::lock_guard<spinlock_t> guard(spl);
        v_t = v_other;
        return *this;
    };
    //
    //-------------------------------------------------------------------------
    //  function : clear
    /// @brief Delete all the elements of the stack_cnc.
    //-------------------------------------------------------------------------
    void clear(void)
    {
        std::lock_guard<spinlock_t> guard(spl);
        v_t.clear();
    };
    //
    //-------------------------------------------------------------------------
    //  function : swap
    /// @brief swap the data between the two stack_cnc
    /// @param A : stack_cnc to swap
    //-------------------------------------------------------------------------
    void swap(stack_cnc &stack_other) noexcept
    {   
        if (this == &stack_other) return;
        std::lock_guard<spinlock_t> guard(spl);
        v_t.swap(stack_other.v_t);
    };
    //
    //-------------------------------------------------------------------------
    //  function : size
    /// @brief return the number of elements in the stack_cnc
    /// @return number of elements in the stack_cnc
    //-------------------------------------------------------------------------
    size_type size(void) const noexcept
    {
        std::lock_guard<spinlock_t> guard(spl);
        return v_t.size();
    };
    //
    //-------------------------------------------------------------------------
    //  function :max_size
    /// @brief return the maximun size of the container
    /// @return maximun size of the container
    //-------------------------------------------------------------------------
    size_type max_size(void) const noexcept
    {
        std::lock_guard<spinlock_t> guard(spl);
        return (v_t.max_size());
    };
    //
    //-------------------------------------------------------------------------
    //  function : shrink_to_fit
    /// @brief Requests the removal of unused capacity.
    ///        It is a non-binding request to reduce capacity() to size().
    ///        It depends on the implementation if the request is fulfilled.
    ///        All iterators, including the past the end iterator, are
    ///        potentially
    ///        invalidated.
    /// @return none
    //-------------------------------------------------------------------------
    void shrink_to_fit()
    {
        std::lock_guard<spinlock_t> guard(spl);
        v_t.shrink_to_fit();
    };
    //
    //-------------------------------------------------------------------------
    //  function : capacity
    /// @brief return the maximun size of the container
    /// @return maximun size of the container
    //-------------------------------------------------------------------------
    size_type capacity(void) const noexcept
    {
        std::lock_guard<spinlock_t> guard(spl);
        return (v_t.capacity());
    };
    //
    //-------------------------------------------------------------------------
    //  function : empty
    /// @brief indicate if the stack_cnc is empty
    /// @return true if the stack_cnc is empty, false in any other case
    //-------------------------------------------------------------------------
    bool empty(void) const noexcept
    {
        std::lock_guard<spinlock_t> guard(spl);
        return (v_t.empty());
    };
    //
    //-------------------------------------------------------------------------
    //  function : reserve
    /// @brief Change the capacity of the container, at least n elements
    /// @param n : number of elements of the new capacity
    //-------------------------------------------------------------------------
    void reserve(size_type n) noexcept
    {
        std::lock_guard<spinlock_t> guard(spl);
        v_t.reserve(n);
    };
    //
    //-------------------------------------------------------------------------
    //  function : copy
    /// @brief explicit copy for to prevent the automatic copy with the
    /// operator=
    /// @param v_other : vector to copy
    //-------------------------------------------------------------------------
    template <class Allocator2>
    void copy(std::vector<value_type, Allocator2> &v_other)
    {
        std::lock_guard<spinlock_t> guard(spl);
        v_other = v_t;
    };

    //-------------------------------------------------------------------------
    //  function : push_back
    /// @brief Insert one element in the back of the container
    /// @param val : value to insert
    //-------------------------------------------------------------------------
    void push_back(const value_type &val)
    {
        std::lock_guard<spinlock_t> guard(spl);
        v_t.push_back(val);
    };

    //-------------------------------------------------------------------------
    //  function : emplace_back
    /// @brief Insert one element in the back of the container
    /// @param args : group of arguments for to build the object to insert. Can
    ///               be values, references or rvalues
    //-------------------------------------------------------------------------
    template <class... Args>
    void emplace_back(Args &&... args)
    {
        std::lock_guard<spinlock_t> guard(spl);
        v_t.emplace_back(std::forward<Args>(args)...);
    };
    //-------------------------------------------------------------------------
    //  function : push_back
    /// @brief Insert one vector at the end of the container
    /// @param v_other : vector to insert
    /// @return reference to the stack_cnc after the insertion
    //-------------------------------------------------------------------------
    template <class Allocator2>
    stack_cnc &push_back(const std::vector<value_type, Allocator2> &v_other)
    {
        std::lock_guard<spinlock_t> guard(spl);
        for (size_type i = 0; i < v_other.size(); ++i) {
            v_t.push_back(v_other[i]);
        }
        return *this;
    };
    //
    //-------------------------------------------------------------------------
    //  function :pop_back
    /// @brief erase the last element of the container
    //-------------------------------------------------------------------------
    void pop_back(void)
    {
        std::lock_guard<spinlock_t> guard(spl);
        v_t.pop_back();
    };

    //
    //-------------------------------------------------------------------------
    //  function :pop_move_back
    /// @brief if exist, move the last element to P, and delete it 
    /// @param P : reference to a variable where move the element
    /// @return  true  - Element moved and deleted
    ///          false - Empty stack_cnc
    //-------------------------------------------------------------------------
    bool pop_move_back(value_type &P)
    { 
        std::lock_guard<spinlock_t> S(spl);
        if (v_t.size() == 0) return false;
        P = std::move(v_t.back());
        v_t.pop_back();
        return true;
    };

}; // end class stack_cnc

//***************************************************************************
}; // end namespace util
}; // end namespace detail
}; // end namespace parallel
}; // end namespace sort
}; // end namespace boost
//***************************************************************************
#endif
