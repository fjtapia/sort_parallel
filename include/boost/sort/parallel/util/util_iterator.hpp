//----------------------------------------------------------------------------
/// @file util_iterator.hpp
/// @brief Utility functions for the iterators
/// @author Copyright (c) 2013 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_UTIL_UTIL_ITERATOR_HPP
#define __BOOST_SORT_PARALLEL_UTIL_UTIL_ITERATOR_HPP

#include <cassert>
#include <utility>
#include <type_traits>
#include <iterator>
#include <functional>
#include <boost/sort/parallel/util/config.hpp>
#include <boost/sort/parallel/util/definition.hpp>

namespace boost
{
namespace sort
{
namespace parallel
{
namespace util
{
//
//############################################################################
//                                                                          ##
//                         T R A I T S                                      ##
//                                                                          ##
//                  M E T A P R O G R A M M I N G                           ##
//                                                                          ##
//############################################################################
//
//---------------------------------------------------------------------------
//         D E F I N I T I O N S
//---------------------------------------------------------------------------
///---------------------------------------------------------------------------
/// @struct iter_value
/// @brief This is for to obtain the type of data pointed by an iterator
/// @tparam iter_t type of iterator
/// @remarks
//----------------------------------------------------------------------------
template <typename iter_t>
struct iter_value
{   typedef typename
    std::remove_reference<decltype(*(std::declval<iter_t>()))>::type type ;
};
//
//##########################################################################
//                                                                        ##
//         S T R U C T     L E S S _ P T R _ N O _ N U L L                ##
//                                                                        ##
//##########################################################################
//
//---------------------------------------------------------------------------
/// @class less_ptr_no_null
///
/// @remarks this is the comparison object for pointers. Receive a object
///          for to compare the objects pointed. The pointers can't be nullptr
//---------------------------------------------------------------------------
template    <   class iter_t ,
                class comp_t =std::less<typename iter_value<iter_t>::type>
            >
struct less_ptr_no_null
{   //----------------------------- Variables -----------------------
    comp_t comp ;
    //----------------------------- Functions ----------------------
    inline less_ptr_no_null ( comp_t C1 = comp_t()):comp(C1){};
    inline bool operator ()( iter_t  T1,  iter_t  T2 ) const
    {   //-------------------- begin ------------------------------
        return  comp(*T1 ,*T2);
    };
};
//****************************************************************************
}; //   Fin namespace util
}; //   Fin namespace parallel
}; //   Fin namespace sort
}; //   Fin namespace boost
//****************************************************************************
#endif


