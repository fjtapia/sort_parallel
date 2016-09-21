//----------------------------------------------------------------------------
/// @file test_insertion_sort.cpp
/// @brief Test program of the insertion_sort algorithm
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include "boost/sort/parallel/detail/insertion_sort.hpp"
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

using namespace boost::sort::parallel::detail;
using namespace std;

void test01 (void)
{
    unsigned A[] = {7,  4, 23, 15, 17, 2, 24, 13, 8,  3,  11,
                    16, 6, 14, 21, 5,  1, 12, 19, 22, 25, 8};

    std::less< unsigned > comp;
    // Insertion Sort  Unordered, not repeated
    insertion_sort (&A[ 0 ], &A[ 22 ], comp);
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK (A[ i ] <= A[ i + 1 ]);
    };

    unsigned B[] = {1,  2,  3,  5,  6,  7,  8,  9,  10, 11, 12,
                    13, 14, 15, 17, 18, 19, 20, 21, 23, 24, 25};
    // Insertion Sort  Ordered, not repeated
    insertion_sort (&B[ 0 ], &B[ 22 ], comp);
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK (B[ i ] <= B[ i + 1 ]);
    };

    unsigned C[] = {27, 26, 25, 23, 22, 21, 19, 18, 17, 16, 15,
                    14, 13, 11, 10, 9,  8,  7,  6,  5,  3,  2};
    // Insertion Sort reverse sorted, not repeated
    insertion_sort (&C[ 0 ], &C[ 22 ], comp);
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK (C[ i ] <= C[ i + 1 ]);
    };

    unsigned D[] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
    // Insertion Sort  equal elements
    insertion_sort (&D[ 0 ], &D[ 22 ], comp);
    for (unsigned i = 0; i < 21; i++) {
        BOOST_CHECK (D[ i ] <= D[ i + 1 ]);
    };

    // Insertion Sort  100 random elements
    unsigned F[ 100 ];
    for (unsigned i = 0; i < 100; i++) F[ i ] = rand ( ) % 1000;
    insertion_sort (&F[ 0 ], &F[ 100 ], comp);
    for (unsigned i = 0; i < 99; i++) {
        BOOST_CHECK (F[ i ] <= F[ i + 1 ]);
    };

    const unsigned NG = 10000;
    // Insertion Sort  "<<NG<<" random elements
    unsigned G[ NG ];
    for (unsigned i = 0; i < NG; i++) G[ i ] = rand ( ) % 1000;
    insertion_sort (&G[ 0 ], &G[ NG ], comp);
    for (unsigned i = 0; i < NG - 1; i++) {
        BOOST_CHECK (G[ i ] <= G[ i + 1 ]);
    };
};
void test02 (void)
{
    typedef typename std::vector< uint64_t >::iterator iter_t;
    const uint32_t NELEM = 6667;
    std::vector< uint64_t > A;
    std::less< uint64_t > comp;

    for (uint32_t i = 0; i < 1000; ++i) A.push_back (0);
    for (uint32_t i = 0; i < NELEM; ++i) A.push_back (NELEM - i);
    for (uint32_t i = 0; i < 1000; ++i) A.push_back (0);

    insertion_sort (A.begin ( ) + 1000, A.begin ( ) + (1000 + NELEM), comp);

    for (iter_t it = A.begin ( ) + 1000; it != A.begin ( ) + (1000 + NELEM);
         ++it)
    {
        BOOST_CHECK ((*(it - 1)) <= (*it));
    };
    BOOST_CHECK (A[ 998 ] == 0 and A[ 999 ] == 0 and A[ 1000 + NELEM ] == 0 and
                 A[ 1001 + NELEM ] == 0);


    A.clear ( );
    for (uint32_t i = 0; i < 1000; ++i) A.push_back (999999999);
    for (uint32_t i = 0; i < NELEM; ++i) A.push_back (NELEM - i);
    for (uint32_t i = 0; i < 1000; ++i) A.push_back (999999999);

    insertion_sort (A.begin ( ) + 1000, A.begin ( ) + (1000 + NELEM), comp);

    for (iter_t it = A.begin ( ) + 1001; it != A.begin ( ) + (1000 + NELEM);
         ++it)
    {
        BOOST_CHECK ((*(it - 1)) <= (*it));
    };
    BOOST_CHECK (A[ 998 ] == 999999999 and A[ 999 ] == 999999999 and
                 A[ 1000 + NELEM ] == 999999999 and
                 A[ 1001 + NELEM ] == 999999999);
};
int test_main (int, char *[])
{
    test01 ( );
    test02 ( );
    return 0;
}
