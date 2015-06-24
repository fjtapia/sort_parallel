//----------------------------------------------------------------------------
/// @file definitions.hpp
/// @brief This file contains the data and type definitions needed
///        adapted to the specific compiler
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_GENERAL_UTIL_DEFINITIONS_HPP
#define __BOOST_SORT_GENERAL_UTIL_DEFINITIONS_HPP


#include <stdint.h>
#include <limits>

//##########################################################################
//                                                                        ##
//                   V I S U A L  C + +                                   ##
//                                                                        ##
//##########################################################################

#ifdef _MSC_VER
//---------------------------------------------------------------------------
//        __declspec(noalias)
// noalias means that a function call does not modify or reference visible
// global state and only modifies the memory pointed to directly by pointer
// parameters (first-level indirections).
//----------------------------------------------------------------------------
#define NOALIAS __declspec(noalias)
//#define NOALIAS
//----------------------------------------------------------------------------
//   __declspec(restrict)
//Applied to a function declaration or definition that returns a pointer type
//and tells the compiler that the function returns an object that will not be
//aliased with any other pointers.
//---------------------------------------------------------------------------
#define RESTRICT __declspec(restrict)
//#define RESTRICT
//---------------------------------------------------------------------------
//               throw ()
// tells the compiler that the function does not throw any exceptions. It is
// the equivalent to using __declspec(nothrow).
//----------------------------------------------------------------------------
#define NOEXCEPT

//---------------------------------------------------------------------------
//               constexpr
// tells the compiler that the function always return the same result with
// the same input and can be used in the metaprogramming.
// The compiler generate two copies of the function, one for the compiler in the
// metaprogramming process and other for the program when invoque the function
//----------------------------------------------------------------------------
#define CONSTEXPR

//---------------------------------------------------------------------------
//               assert
// make an assert, posibiliting the susbtitution by any other format af assert
// like the BOOST_ASSERT
//----------------------------------------------------------------------------
#define ASSERT assert

//##########################################################################
//                                                                        ##
//                      G N U     G C C                                   ##
//                                                                        ##
//##########################################################################

#else
#ifdef __GNUC__
//#include <inttypes.h>
//---------------------------------------------------------------------------
//        __declspec(noalias)
// noalias means that a function call does not modify or reference visible
// global state and only modifies the memory pointed to directly by pointer
// parameters (first-level indirections).
//----------------------------------------------------------------------------
#define NOALIAS __restrict

//----------------------------------------------------------------------------
//   __declspec(restrict)
//Applied to a function declaration or definition that returns a pointer type
//and tells the compiler that the function returns an object that will not be
//aliased with any other pointers.
//---------------------------------------------------------------------------
#define RESTRICT __restrict

//---------------------------------------------------------------------------
//               NOEXCEPT
// tells the compiler that the function does not throw any exceptions. It is
// the equivalent to using __declspec(nothrow).
//----------------------------------------------------------------------------
#define NOEXCEPT  noexcept

//---------------------------------------------------------------------------
//               constexpr
// tells the compiler that the function always return the same result with
// the same input and can be used in the metaprogramming.
// The compiler generate two copies of the function, one for the compiler in the
// metaprogramming process and other for the program when invoque the function
//----------------------------------------------------------------------------
#define CONSTEXPR constexpr

//---------------------------------------------------------------------------
//               assert
// make an assert, posibiliting the susbtitution by any other format af assert
// like the BOOST_ASSERT
//----------------------------------------------------------------------------
#define ASSERT assert

//##########################################################################
//                                                                        ##
//                      G E N E R I C                                     ##
//                                                                        ##
//##########################################################################

#else
//---------------------------------------------------------------------------
//        __declspec(noalias)
// noalias means that a function call does not modify or reference visible
// global state and only modifies the memory pointed to directly by pointer
// parameters (first-level indirections).
//----------------------------------------------------------------------------
#define NOALIAS

//----------------------------------------------------------------------------
//   __declspec(restrict)
//Applied to a function declaration or definition that returns a pointer type
//and tells the compiler that the function returns an object that will not be
//aliased with any other pointers.
//---------------------------------------------------------------------------
#define RESTRICT

//---------------------------------------------------------------------------
//               throw ()
// tells the compiler that the function does not throw any exceptions. It is
// the equivalent to using __declspec(nothrow).
//----------------------------------------------------------------------------
#define NOEXCEPT  noexcept

//---------------------------------------------------------------------------
//               constexpr
// tells the compiler that the function always return the same result with
// the same input and can be used in the metaprogramming.
// The compiler generate two copies of the function, one for the compiler in the
// metaprogramming process and other for the program when invoque the function
//----------------------------------------------------------------------------
#define CONSTEXPR constexpr

//---------------------------------------------------------------------------
//               assert
// make an assert, posibiliting the susbtitution by any other format af assert
// like the BOOST_ASSERT
//----------------------------------------------------------------------------
#define ASSERT assert
#endif
#endif
#endif
