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
#include <boost/sort/parallel/detail/util/stack_cnc.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

namespace bsp_util = boost::sort::parallel::detail::util;
using std::cout;
using std::endl;
using bsp_util::stack_cnc;

#define NELEM 5000000

static const uint32_t NC = std::thread::hardware_concurrency();
static const uint32_t NCores = (NC > 8) ? 8 : NC;
stack_cnc<int> S;

void function1();
void function2();
void test_cnc(void);
void test(void);

int test_main(int, char *[])
{
    test();
    test_cnc();
    return 0;
};

void function1()
{
    for (int i = 0; i < NELEM; ++i) S.push_back(i);
};
void function2()
{
    int k;
    while (S.pop_move_back(k));
};

void test_cnc(void)
{
    //------------ begin ------------------
    S.reserve((int)NELEM * (int)NCores);
    // double duracion;

    std::thread T[8];

    for (uint32_t i = 0; i < NCores; ++i) {
        T[i] = std::thread(function1);
    };
    for (uint32_t i = 0; i < NCores; ++i) {
        T[i].join();
    };

    for (uint32_t i = 0; i < NCores; ++i) {
        T[i] = std::thread(function2);
    };
    for (uint32_t i = 0; i < NCores; ++i) {
        T[i].join();
    };
};

void test(void)
{
    //------------- begin -------------
    stack_cnc<int> A;
    std::vector<int> V1 = {0, 1, 2};
    stack_cnc<int> B(V1);
    V1.clear();

    BOOST_CHECK(V1.size() == 0);
    B.copy(V1);
    for (int i = 0; i < 3; ++i) BOOST_CHECK(V1[i] == i);
    B.clear();

    BOOST_CHECK(B.size() == 0);
    B.push_back(V1);
    BOOST_CHECK(B.size() == 3);
    B.push_back(3);
    B.emplace_back(4);
    BOOST_CHECK(B.size() == 5);

    B.copy(V1);
    for (int i = 0; i < 5; ++i) BOOST_CHECK(V1[i] == i);
    V1.clear();

    int z;
    BOOST_CHECK(B.pop_move_back(z) and z == 4);
    V1.resize(2);
    BOOST_CHECK(B.pop_move_back(V1[1]));
    BOOST_CHECK(B.pop_move_back(V1[0]));
    BOOST_CHECK(V1.size() == 2 and V1[0] == 2 and V1[1] == 3);
    BOOST_CHECK(B.size() == 2);

    B.pop_back();
    BOOST_CHECK(B.size() == 1);
    B.copy(V1);
    BOOST_CHECK(V1.size() == 1 and V1[0] == 0);
}
