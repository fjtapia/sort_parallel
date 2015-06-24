//----------------------------------------------------------------------------
/// @file config.hpp
/// @brief
///
/// @author Copyright (c) 2010 2013 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_GENERAL_UTIL_CONFIG_HPP
#define __BOOST_SORT_GENERAL_UTIL_CONFIG_HPP


#include <cstdlib>
#include <limits>
#include <ciso646>
#include <cassert>

namespace boost
{
namespace sort
{
namespace parallel
{
namespace util
{

//-----------------------------------------------------------------
// Public data definitions
//-----------------------------------------------------------------
const unsigned NByte = sizeof(size_t) ;
//----------------------------------------------------------
//    32 bits compiler
//----------------------------------------------------------
template <unsigned NByte >
struct Conf
{   //--------------- Type definition--------------
    typedef uint32_t size_type;
    typedef int32_t  difference_type ;
    typedef uint32_t bitfield_t ;
    static const uint32_t  n_bits_number = 31 ;
    static const uint32_t  n_bits_max = 31 ;
    static const uint32_t  MAX = 0xFFFFFFFFU ;
    //enum { n_bits_number = 31 };
};
//----------------------------------------------------------
//    64 bits compiler
//----------------------------------------------------------
template < >
struct Conf<8>
{   //------Type definition -------------
    typedef uint64_t size_type;
    typedef int64_t  difference_type;
    typedef uint64_t bitfield_t ;
    static const uint32_t n_bits_number = 63;

#if defined (_MSC_VER)
    static const uint32_t n_bits_max = 31;
#else
    static const uint32_t n_bits_max = 63;
#endif
    static const uint64_t MAX = 0xFFFFFFFFFFFFFFFFULL ;
};
//---------------------------------------------------------------------------
//             automatic selection
//--------------------------------------------------------------------------
typedef Conf<NByte>::size_type       size_type ;
typedef Conf<NByte>::difference_type difference_type ;

typedef Conf<NByte>::size_type       unsigned_type ;
typedef Conf<NByte>::difference_type signed_type ;
static const uint32_t n_bits_max = Conf<NByte>::n_bits_max ;

static const uint64_t MAX64 = Conf<8>::MAX ;
static const uint32_t MAX32 = Conf<4>::MAX ;

static const size_type MAX = Conf<NByte>::MAX ;

//****************************************************************************
}; //   End namespace util
}; //   End namespace parallel
}; //   End namespace sort
}; //   End namespace boost
//****************************************************************************
#endif

