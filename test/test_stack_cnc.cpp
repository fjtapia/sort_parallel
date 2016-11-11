//----------------------------------------------------------------------------
/// @file test_stack_cnc.cpp
/// @brief Test program of the class spinlock
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
#include <chrono>
#include <mutex>
#include <thread>
#include <boost/sort/parallel/detail/util/stack_cnc.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>

namespace bsp_util = boost::sort::parallel::detail::util;
using bsp_util::stack_cnc;

#define NELEM 5000000

static const uint32_t NC = std::thread::hardware_concurrency ( );
static const uint32_t NCores = (NC > 8) ? 8 : NC;
stack_cnc< int > S;

void function1 ( );
void function2 ( );
void test_cnc (void);
void test (void);

int test_main (int, char *[])
{
    test ( );
    test_cnc ( );
    return 0;
};

void function1 ( )
{
    for (int i = 0; i < NELEM; ++i) S.emplace_back (i);
};
void function2 ( )
{
    int k;
    while (S.pop_move_back (k))
        ;
};

void test_cnc (void)
{
    std::thread T[8];

    for (uint32_t i = 0; i < NCores; ++i) {
        T[i] = std::thread (function1);
    };
    for (uint32_t i = 0; i < NCores; ++i) {
        T[i].join ( );
    };

    for (uint32_t i = 0; i < NCores; ++i) {
        T[i] = std::thread (function2);
    };
    for (uint32_t i = 0; i < NCores; ++i) {
        T[i].join ( );
    };
};

void test (void)
{
    stack_cnc< int > A;
    std::vector< int > B = {0, 1, 2};

    for (int i = 0; i < 3; ++i) BOOST_CHECK (B[i] == i);

    B.emplace_back (3);
    B.emplace_back (4);

    for (int i = 0; i < 5; ++i) BOOST_CHECK (B[i] == i);
}
