//----------------------------------------------------------------------------
/// @file test_heap_sort.cpp
/// @brief Test program of the heap sort algorithm
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include "boost/sort/parallel/detail/heap_sort.hpp"
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

using namespace boost::sort::parallel::detail;
using namespace std;

void test2()
{ //---------------------------- Inicio --------------------------------
    typedef less<unsigned> Compare;
    Compare comp;
    unsigned A[] = {7,  4, 23, 15, 17, 2, 24, 13, 8,  3,  11,
                    16, 6, 14, 21, 5,  1, 12, 19, 22, 25, 8};

    // Heap Sort  Unordered not repeated
    heap_sort<unsigned *, Compare>(&A[0], &A[22], comp);
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK(A[i] <= A[i + 1]);
    };

    unsigned B[] = {1,  2,  3,  5,  6,  7,  8,  9,  10, 11, 12,
                    13, 14, 15, 17, 18, 19, 20, 21, 23, 24, 25};
    // Heap Sort Sorted, not repeated
    heap_sort<unsigned *, Compare>(&B[0], &B[22], comp);
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK(B[i] <= B[i + 1]);
    };

    unsigned C[] = {27, 26, 25, 23, 22, 21, 19, 18, 17, 16, 15,
                    14, 13, 11, 10, 9,  8,  7,  6,  5,  3,  2};
    // Heap Sort reverse sorted, not repeated
    heap_sort<unsigned *, Compare>(&C[0], &C[22], comp);
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK(C[i] <= C[i + 1]);
    };

    unsigned D[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
    // Heap Sort equals
    heap_sort<unsigned *, Compare>(&D[0], &D[22], comp);
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK(D[i] >= D[i + 1]);
    };

    // Heap Sort 100 random numbers
    unsigned F[100];
    for (unsigned i = 0; i < 100; i++) F[i] = rand() % 1000;
    heap_sort<unsigned *, Compare>(&F[0], &F[100], comp);
    for (unsigned i = 0; i < 99; i++) {
        BOOST_CHECK(F[i] <= F[i + 1]);
    };

    const unsigned NG = 10000;
    // Heap Sort NG random numbers
    unsigned G[NG];
    for (unsigned i = 0; i < NG; i++) G[i] = rand() % 1000;
    heap_sort<unsigned *, Compare>(&G[0], &G[NG], comp);
    for (unsigned i = 0; i < NG - 1; i++) {
        BOOST_CHECK(G[i] <= G[i + 1]);
    };
};

void test3(void)
{ //---------------------- begin ------------------------------------
    typedef std::less<uint32_t> Compare;
    Compare comp;
    const uint32_t NELEM = 100000;
    std::vector<uint32_t> A;
    A.reserve(NELEM);

    for (unsigned i = 0; i < NELEM; i++) A.push_back(rand());

    heap_sort<uint32_t *, Compare>(&A[0], &A[NELEM], comp);
    for (unsigned i = 0; i < (NELEM - 1); i++) {
        BOOST_CHECK(A[i] <= A[i + 1]);
    };
}
int test_main(int, char *[])
{
    test2();
    test3();
    return 0;
};
