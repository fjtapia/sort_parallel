//----------------------------------------------------------------------------
/// @file test_sample_sort.cpp
/// @brief test sample_sort algorithm
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
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>
#include <random>
#include <boost/sort/parallel/detail/sample_sort.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>

using namespace boost::sort::parallel::detail;
using namespace boost::sort::parallel::detail::util;

std::mt19937_64 my_rand (0);

void test3 ( );
void test4 ( );
void test5 ( );
void test6 ( );
void test7 ( );

struct xk
{
    unsigned tail : 3;
    unsigned num : 24;
    bool operator< (xk A) const { return (num < A.num); };
};

void test3 ( )
{
    typedef typename std::vector< xk >::iterator iter_t;
    typedef std::less< xk > compare;

    std::mt19937_64 my_rand (0);

    const uint32_t NMAX = 500000;

    std::vector< xk > V1, V2, V3;
    V1.reserve (NMAX);
    for (uint32_t i = 0; i < 8; ++i) {
        for (uint32_t k = 0; k < NMAX; ++k) {
            uint32_t NM = my_rand ( );
            xk G;
            G.num = NM >> 3;
            G.tail = i;
            V1.push_back (G);
        };
    };
    V3 = V2 = V1;
    sample_sort< iter_t, compare > (V1.begin ( ), V1.end ( ));
    std::stable_sort (V2.begin ( ), V2.end ( ));
    sample_sort< iter_t, compare > (V3.begin ( ), V3.end ( ), 0);

    BOOST_CHECK (V1.size ( ) == V2.size ( ));
    for (uint32_t i = 0; i < V1.size ( ); ++i) {
        BOOST_CHECK (V1[ i ].num == V2[ i ].num and
                     V1[ i ].tail == V2[ i ].tail);
    };

    BOOST_CHECK (V3.size ( ) == V2.size ( ));
    for (uint32_t i = 0; i < V3.size ( ); ++i) {
        BOOST_CHECK (V3[ i ].num == V2[ i ].num and
                     V3[ i ].tail == V2[ i ].tail);
    };
};

void test4 (void)
{
    typedef typename std::vector< uint64_t >::iterator iter_t;
    typedef std::less< uint64_t > compare;

    const uint32_t NElem = 500000;
    std::vector< uint64_t > V1;
    std::mt19937_64 my_rand (0);

    for (uint32_t i = 0; i < NElem; ++i) V1.push_back (my_rand ( ) % NElem);

    // sample_sort unsorted
    sample_sort< iter_t, compare > (V1.begin ( ), V1.end ( ));
    for (unsigned i = 1; i < NElem; i++) {
        BOOST_CHECK (V1[ i - 1 ] <= V1[ i ]);
    };

    V1.clear ( );
    for (uint32_t i = 0; i < NElem; ++i) V1.push_back (i);

    // sample_sort sorted
    sample_sort< iter_t, compare > (V1.begin ( ), V1.end ( ));
    for (unsigned i = 1; i < NElem; i++) {
        BOOST_CHECK (V1[ i - 1 ] <= V1[ i ]);
    };

    V1.clear ( );
    for (uint32_t i = 0; i < NElem; ++i) V1.push_back (NElem - i);

    // sample_sort reverse sorted
    sample_sort< iter_t, compare > (V1.begin ( ), V1.end ( ));
    for (unsigned i = 1; i < NElem; i++) {
        BOOST_CHECK (V1[ i - 1 ] <= V1[ i ]);
    };

    V1.clear ( );
    for (uint32_t i = 0; i < NElem; ++i) V1.push_back (1000);

    // sample_sort equals
    sample_sort< iter_t, compare > (V1.begin ( ), V1.end ( ));
    for (unsigned i = 1; i < NElem; i++) {
        BOOST_CHECK (V1[ i - 1 ] == V1[ i ]);
    };
};

void test5 (void)
{
    typedef typename std::vector< uint64_t >::iterator iter_t;
    typedef std::less< uint64_t > compare;
    const uint32_t KMax = 500000;

    std::vector< uint64_t > K, M;
    std::mt19937_64 my_rand (0);
    std::less< uint64_t > comp;

    for (uint32_t i = 0; i < KMax; ++i) K.push_back (my_rand ( ));
    M = K;

    // sample_sort - random elements
    uint64_t *Ptr = std::get_temporary_buffer< uint64_t > (KMax).first;
    if (Ptr == nullptr) throw std::bad_alloc ( );
    range< uint64_t * > Rbuf (Ptr, Ptr + KMax);

    sample_sort< iter_t, compare > (K.begin ( ), K.end ( ), comp,
                                    std::thread::hardware_concurrency ( ),
                                    Rbuf);

    std::return_temporary_buffer (Ptr);

    std::stable_sort (M.begin ( ), M.end ( ), comp);
    for (unsigned i = 0; i < KMax; i++) BOOST_CHECK (M[ i ] == K[ i ]);
};

void test6 (void)
{
    typedef typename std::vector< uint64_t >::iterator iter_t;
    typedef std::less< uint64_t > compare;
    std::vector< uint64_t > V;

    for (uint32_t i = 0; i < 2083333; ++i) V.push_back (i);
    sample_sort< iter_t, compare > (V.begin ( ), V.end ( ));
    for (uint32_t i = 0; i < V.size ( ); ++i) {
        BOOST_CHECK (V[ i ] == i);
    };
};

void test7 (void)
{
    typedef typename std::vector< uint64_t >::iterator iter_t;
    typedef std::less< uint64_t > compare;

    const uint32_t NELEM = 416667;
    std::vector< uint64_t > A;

    for (uint32_t i = 0; i < 1000; ++i) A.push_back (0);
    for (uint32_t i = 0; i < NELEM; ++i) A.push_back (NELEM - i);
    for (uint32_t i = 0; i < 1000; ++i) A.push_back (0);

    sample_sort< iter_t, compare > (A.begin ( ) + 1000,
                                    A.begin ( ) + (1000 + NELEM));

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

    sample_sort< iter_t, compare > (A.begin ( ) + 1000,
                                    A.begin ( ) + (1000 + NELEM));

    for (iter_t it = A.begin ( ) + 1001; it != A.begin ( ) + (1000 + NELEM);
         ++it)
    {
        BOOST_CHECK ((*(it - 1)) <= (*it));
    };
    BOOST_CHECK (A[ 998 ] == 999999999 and A[ 999 ] == 999999999 and
                 A[ 1000 + NELEM ] == 999999999 and
                 A[ 1001 + NELEM ] == 999999999);

    std::vector< uint64_t > B (NELEM + 2000, 0);

    A.clear ( );
    for (uint32_t i = 0; i < NELEM; ++i) A.push_back (NELEM - i);

    range< uint64_t * > Rbuf (&B[ 1000 ], (&B[ 1000 ]) + NELEM);
    sample_sort< iter_t, compare > (
        A.begin ( ), A.end ( ), std::less< uint64_t > ( ),
        std::thread::hardware_concurrency ( ), Rbuf);

    for (iter_t it = A.begin ( ) + 1; it != A.end ( ); ++it) {
        BOOST_CHECK ((*(it - 1)) <= (*it));
    };
    BOOST_CHECK (B[ 998 ] == 0 and B[ 999 ] == 0 and B[ 1000 + NELEM ] == 0 and
                 B[ 1001 + NELEM ] == 0);

    for (uint32_t i = 0; i < B.size ( ); ++i) B[ i ] = 999999999;
    A.clear ( );
    for (uint32_t i = 0; i < NELEM; ++i) A.push_back (NELEM - i);

    sample_sort< iter_t, std::less< uint64_t > > (
        A.begin ( ), A.end ( ), std::less< uint64_t > ( ),
        std::thread::hardware_concurrency ( ), Rbuf);

    for (iter_t it = A.begin ( ) + 1; it != A.end ( ); ++it) {
        if ((*(it - 1)) > (*it)) std::cout << "error 2\n";
    };
    BOOST_CHECK (B[ 998 ] == 999999999 and B[ 999 ] == 999999999 and
                 B[ 1000 + NELEM ] == 999999999 and
                 B[ 1001 + NELEM ] == 999999999);
};
void test8 (void)
{
    typedef typename std::vector< uint64_t >::iterator iter_t;
    typedef std::less< uint64_t > compare;
    const uint32_t KMax = 66000;

    std::vector< uint64_t > K, M;
    std::mt19937_64 my_rand (0);
    std::less< uint64_t > comp;

    for (uint32_t i = 0; i < KMax; ++i) K.push_back (my_rand ( ));
    M = K;

    sample_sort< iter_t, compare > (K.begin ( ), K.end ( ), 300);

    std::stable_sort (M.begin ( ), M.end ( ), comp);
    for (unsigned i = 0; i < KMax; i++) BOOST_CHECK (M[ i ] == K[ i ]);
};
int test_main (int, char *[])
{
    test3 ( );
    test4 ( );
    test5 ( );
    test6 ( );
    test7 ( );
    test8 ( );
    return 0;
};
