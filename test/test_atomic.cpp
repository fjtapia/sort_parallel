//----------------------------------------------------------------------------
/// @file test_atomic.cpp
/// @brief Test program of the functions for to manipulate atomic variables
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <ciso646>
#include <boost/sort/parallel/detail/util/atomic.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>

namespace bsp_util = boost::sort::parallel::detail::util;

int test_main (int, char *[])
{
    std::atomic< uint32_t > A (7);

    std::atomic_uint B (6);
    auto X =
        std::atomic_load_explicit< uint32_t > (&A, std::memory_order_acquire);

    BOOST_CHECK (X == 7);
    X = bsp_util::atomic_read< uint32_t > (A);
    BOOST_CHECK (X == 7);
    return 0;
};
