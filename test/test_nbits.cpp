//----------------------------------------------------------------------------
/// @file test_nbits.cpp
/// @brief Test program of the nbits functions
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <boost/sort/parallel/tools/nbits.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>

using namespace boost::sort::parallel::tools;

int test_main(int, char *[])
{
    //--------------------------------------------------------------------
    // NBits
    //--------------------------------------------------------------------
    BOOST_CHECK(nbits32(63UL) == 6);
    BOOST_CHECK(nbits32(64UL) == 7);
    BOOST_CHECK(nbits32(65UL) == 7);
    //--------------------------------------------------------------------
    // NBits
    //--------------------------------------------------------------------
    BOOST_CHECK(nbits64(63ULL) == 6);
    BOOST_CHECK(nbits64(64ULL) == 7);
    BOOST_CHECK(nbits64(65ULL) == 7);
    return 0;
};
