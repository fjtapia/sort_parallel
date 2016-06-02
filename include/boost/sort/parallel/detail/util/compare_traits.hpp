//----------------------------------------------------------------------------
/// @file compare_trait.hpp
/// @brief This class is the metaprogramming implem,entacion odf a selector of
///        the compare type in the sort algorithms invocation
/// @author Copyright(c) 2016 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_DETAIL_UTIL_COMPARE_TRAITS_HPP
#define __BOOST_SORT_PARALLEL_DETAIL_UTIL_COMPARE_TRAITS_HPP

#include <type_traits>
#include <functional>
#include <iterator>

namespace boost		{
namespace sort		{
namespace parallel	{
namespace detail	{
namespace util		{

using std::iterator_traits ;
//---------------------------------------------------------------------------
/// @class comp_unknown
/// @brief In the parallel sorting algorithms, the two first parameters are
///        the iterators to the first and the last element to sort.
///        In the invocation with 4 parameter , the third is the comparison
///        object , and the last is the number of threads to use .
///        But in the invocation with 3 parameters, the third can be the
///        comparison object, and the threads are the HW threads in the machine.
///        or can be the number of threads, and then, comparison is done with
///        the std::less <value_t>
///        This class detect when the parameter is integral
///        If true, consider as the number of threads and the value_type is
///        the std::less of the value_t
///        If false ,consider as the comparison object and the value_type is
///        the same type than unknown
/// @remarks The empty construct load the object with the number of HW threads
///          in the actual machine
//---------------------------------------------------------------------------

template <class iter_t, class unknown, bool b=std::is_integral<unknown>::value>
struct comp_unknown;

template <class iter_t , class unknown >
struct comp_unknown <iter_t, unknown, true>
{   typedef std::less<typename std::iterator_traits<iter_t>::value_type>
                                                                   value_type ;
};

template <class iter_t , class unknown >
struct comp_unknown <iter_t, unknown, false>
{   typedef unknown value_type ;
};

template <class iter_t>
using compare_iter = std::less< typename iterator_traits<iter_t>::value_type > ;

//****************************************************************************
}; // End namespace util
}; // End namespace detail
}; // End namespace parallel
}; // End namespace sort
}; // End namespace boost
//****************************************************************************
#endif
