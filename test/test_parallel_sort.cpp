//----------------------------------------------------------------------------
/// @file test_parallel_sort.cpp
/// @brief
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <algorithm>
#include <boost/sort/parallel/detail/parallel_sort.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <cassert>
#include <iostream>
#include <random>
#include <stdlib.h>

using boost::sort::parallel::detail::parallel_sort;
using namespace std;

#define NELEM 5000000

void Test3();

vector<uint32_t> A;

int test_main(int, char *[])
{ //--------------------------- Inicio --------------------------
    std::mt19937_64 my_rand(0);
    A.reserve(NELEM);

    //------------------------ Inicio -----------------------------
    // Sort of NELEM elements in a vector

    // Sorted elements
    A.clear();
    for (uint32_t i = 0; i < NELEM; i++) A.push_back(NELEM + i);
    Test3();

    // Reverse sorted elements
    A.clear();
    for (uint32_t i = 0; i < NELEM; i++) A.push_back((NELEM << 1) - i);
    Test3();

    // Random elements, few repeated
    A.clear();
    for (uint32_t i = 0; i < NELEM; i++) A.push_back(my_rand());
    Test3();

    // Random elements, quite repeated
    A.clear();
    for (uint32_t i = 0; i < NELEM; i++) A.push_back(my_rand() % (NELEM / 2));
    Test3();

    // Random element many repeated
    A.clear();
    for (uint32_t i = 0; i < NELEM; i++) A.push_back(my_rand() % 10000);
    Test3();

    // Equal elements
    A.clear();
    for (uint32_t i = 0; i < NELEM; i++) A.push_back(NELEM);
    Test3();
    return 0;
};

void Test3(void)
{ //---------------------------- begin --------------------------------
    typedef typename std::vector<uint32_t>::iterator iter_t;
    typedef std::less<uint32_t> compare;
    std::vector<uint32_t> B;
    B.reserve(NELEM);

    // --------------------- parallel_sort ------------------------
    B = A;
    parallel_sort<iter_t, compare>(B.begin(), B.end());
    for (unsigned i = 0; i < (B.size() - 1); i++) {
        BOOST_CHECK(B[i] <= B[i + 1]);
    };
    std::sort(A.begin(), A.end());
    for (unsigned i = 0; i < A.size(); i++) {
        BOOST_CHECK(B[i] == A[i]);
    };
};
