//----------------------------------------------------------------------------
/// @file test_merge_vector.cpp
/// @brief
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <ciso646>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <boost/sort/parallel/detail/util/merge_vector.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>


using namespace boost::sort::parallel::detail::util;

struct xk
{
    unsigned tail : 3;
    unsigned num : 24;
    xk (unsigned N = 0, unsigned T = 0) : tail (T), num (N){};
    bool operator< (xk A) const { return (num < A.num); };
};

//
//----------------------------------------------------------------------------
//               TEST OF  MERGE_LEVEL4
//----------------------------------------------------------------------------
void test1 (void)
{
    uint64_t X[10][10], Y[100];
    range< uint64_t * > RY (&Y[0], &Y[100]);

    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t k = 0; k < 10; ++k) X[i][k] = i + k * 4;
    };
    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t k = 0; k < 10; ++k) X[i + 4][k] = i + 40 + k * 3;
    };
    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t k = 0; k < 10; ++k) X[i + 7][k] = i + 70 + k * 3;
    };

    for (uint32_t i = 0; i < 100; ++i) Y[i] = 1000;
    std::vector< range< uint64_t * > > V, Z;
    for (uint32_t i = 0; i < 10; ++i) V.emplace_back (&X[i][0], &X[i][10]);
    merge_level4 (RY, V, Z, std::less< uint64_t > ( ));

    for (uint32_t i = 0; i < 100; ++i) BOOST_CHECK (Y[i] == i);
    BOOST_CHECK (Z.size ( ) == 3);
};
void test2 (void)
{
    typedef typename std::vector< xk >::iterator iter_t;
    typedef range< iter_t > rng;
    typedef std::less< xk > compare;

    std::vector< xk > VA, VB;
    VB.resize (90);
    rng RB (VB.begin ( ), VB.end ( ));

    for (uint32_t i = 0; i < 30; ++i) {
        uint32_t k = i % 10;
        VA.emplace_back (k, i / 10);
    };
    for (uint32_t i = 0; i < 30; ++i) {
        uint32_t k = i % 10;
        VA.emplace_back (k + 10, i / 10);
    };
    for (uint32_t i = 0; i < 30; ++i) {
        uint32_t k = i % 10;
        VA.emplace_back (k + 20, i / 10);
    };

    std::vector< rng > Vin, Vout;
    for (uint32_t i = 0; i < 9; ++i) {
        Vin.emplace_back (VA.begin ( ) + (i * 10),
                          VA.begin ( ) + ((i + 1) * 10));
    };
    compare comp;
    merge_level4 (RB, Vin, Vout, comp);

    for (uint32_t i = 0; i < 90; ++i) {
        BOOST_CHECK (VB[i].num == i / 3 and VB[i].tail == i % 3);
    };
};

//
//----------------------------------------------------------------------------
//               TEST OF  UNINIT_MERGE_LEVEL4
//----------------------------------------------------------------------------
void test3 (void)
{
    uint64_t X[10][10], Y[100];
    range< uint64_t * > RY (&Y[0], &Y[100]);

    for (uint32_t i = 0; i < 4; ++i) {
        for (uint32_t k = 0; k < 10; ++k) X[i][k] = i + k * 4;
    };
    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t k = 0; k < 10; ++k) X[i + 4][k] = i + 40 + k * 3;
    };
    for (uint32_t i = 0; i < 3; ++i) {
        for (uint32_t k = 0; k < 10; ++k) X[i + 7][k] = i + 70 + k * 3;
    };

    for (uint32_t i = 0; i < 100; ++i) Y[i] = 1000;
    std::vector< range< uint64_t * > > V, Z;
    for (uint32_t i = 0; i < 10; ++i) V.emplace_back (&X[i][0], &X[i][10]);
    uninit_merge_level4 (RY, V, Z, std::less< uint64_t > ( ));

    for (uint32_t i = 0; i < 100; ++i) BOOST_CHECK (Y[i] == i);
    BOOST_CHECK (Z.size ( ) == 3);
};

void test4 (void)
{
    typedef typename std::vector< xk >::iterator iter_t;
    typedef range< iter_t > rng;
    typedef std::less< xk > compare;

    std::vector< xk > VA;
    xk VB[90];
    range< xk * > RB (&VB[0], &VB[89]);

    for (uint32_t i = 0; i < 30; ++i) {
        uint32_t k = i % 10;
        VA.emplace_back (k, i / 10);
    };
    for (uint32_t i = 0; i < 30; ++i) {
        uint32_t k = i % 10;
        VA.emplace_back (k + 10, i / 10);
    };
    for (uint32_t i = 0; i < 30; ++i) {
        uint32_t k = i % 10;
        VA.emplace_back (k + 20, i / 10);
    };

    std::vector< rng > Vin;
    std::vector< range< xk * > > Vout;
    for (uint32_t i = 0; i < 9; ++i) {
        Vin.emplace_back (VA.begin ( ) + (i * 10),
                          VA.begin ( ) + ((i + 1) * 10));
    };
    compare comp;
    uninit_merge_level4 (RB, Vin, Vout, comp);

    for (uint32_t i = 0; i < 90; ++i) {
        BOOST_CHECK (VB[i].num == i / 3 and VB[i].tail == i % 3);
    };
};

//
//----------------------------------------------------------------------------
//                 TEST OF MERGE_VECTOR4
//----------------------------------------------------------------------------
void test5 (void)
{
    typedef typename std::vector< uint64_t >::iterator iter_t;
    typedef range< iter_t > rng;
    typedef std::less< uint64_t > compare;

    std::vector< uint64_t > X, Y;
    compare comp;
    Y.resize (100, 0);

    for (uint32_t i = 0; i < 10; ++i) {
        for (uint32_t k = 0; k < 10; ++k) {
            X.push_back (k * 10 + i);
        }
    };
    rng Rin (X.begin ( ), X.end ( ));
    rng Rout (Y.begin ( ), Y.end ( ));

    std::vector< rng > Vin, Vout;
    for (uint32_t i = 0; i < 10; ++i) {
        Vin.emplace_back (X.begin ( ) + (i * 10), X.begin ( ) + ((i + 1) * 10));
    };
    rng RX (merge_vector4 (Rin, Rout, Vin, Vout, comp));
    BOOST_CHECK (RX.size ( ) == 100);

    for (uint32_t i = 0; i < Y.size ( ); ++i) {
        BOOST_CHECK (Y[i] == i);
    };
};

void test6 (void)
{
    typedef typename std::vector< xk >::iterator iter_t;
    typedef range< iter_t > rng;
    typedef std::less< xk > compare;

    std::vector< xk > VA, VB;
    VB.resize (160);
    compare comp;
    for (uint32_t i = 0; i < 80; ++i) {
        uint32_t k = i % 10;
        VA.emplace_back (2 * k + 1, i / 10);
    };
    for (uint32_t i = 0; i < 80; ++i) {
        uint32_t k = i % 10;
        VA.emplace_back (2 * k, i / 10);
    };
    std::vector< rng > Vin, Vout;
    for (uint32_t i = 0; i < 16; ++i) {
        Vin.emplace_back (VA.begin ( ) + (i * 10),
                          VA.begin ( ) + ((i + 1) * 10));
    };
    rng RA (VA.begin ( ), VA.end ( ));
    rng RB (VB.begin ( ), VB.end ( ));

    rng RX (merge_vector4 (RA, RB, Vin, Vout, comp));
    BOOST_CHECK (RX.size ( ) == 160);

    for (uint32_t i = 0; i < VB.size ( ); ++i) {
        uint32_t K = i / 8;
        BOOST_CHECK (VB[i].num == K and VB[i].tail == (i % 8));
    };
};

int test_main (int, char *[])
{
    test1 ( );
    test2 ( );
    test3 ( );
    test4 ( );
    test5 ( );
    test6 ( );
    return 0;
};
