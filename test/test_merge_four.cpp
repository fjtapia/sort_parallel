//----------------------------------------------------------------------------
/// @file test_merge_four.cpp
/// @brief
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <algorithm>
#include <boost/sort/parallel/detail/util/merge_four.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <stdlib.h>
#include <time.h>
#include <vector>

using namespace boost::sort::parallel::detail::util;

struct xk
{
    unsigned tail : 3;
    unsigned num : 24;
    xk (unsigned N = 0, unsigned T = 0) : tail (T), num (N){};
    bool operator< (xk A) const { return (num < A.num); };
};

void test1 (void)
{
    typedef typename std::vector< uint64_t >::iterator iter_t;
    typedef range< iter_t > range_it;
    typedef std::less< uint64_t > compare;

    std::vector< uint64_t > A, B, C, D, X;
    range_it R[4];
    compare comp;
    range_it RA (A.begin ( ), A.end ( ));
    range_it RB (B.begin ( ), B.end ( ));
    range_it RC (C.begin ( ), C.end ( ));
    range_it RD (D.begin ( ), D.end ( ));
    range_it RX (X.begin ( ), X.end ( ));

    //----------------------------------------------------------------------
    //   0 ranges
    //----------------------------------------------------------------------
    RX = full_merge4 (RX, R, 0, comp);
    BOOST_CHECK (RX.size ( ) == 0);

    //---------------------------------------------------------------------
    // 4 empty ranges
    //---------------------------------------------------------------------
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    BOOST_CHECK (RA.size ( ) == 0 and RB.size ( ) == 0 and RC.size ( ) == 0 and
                 RD.size ( ) == 0);
    RX = full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 0);

    //---------------------------------------------------------------------
    // 1 range filled and 3 empty
    //---------------------------------------------------------------------
    X.resize (10, 0);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i);
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range_it (X.begin ( ), X.end ( ));
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    BOOST_CHECK (RD.size ( ) == 10);
    RX = full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 10);
    for (uint32_t i = 0; i < X.size ( ); ++i) BOOST_CHECK (X[i] == i);

    //---------------------------------------------------------------------
    // Two ranges
    //---------------------------------------------------------------------
    D.clear ( );
    for (uint32_t i = 0; i < 10; ++i) A.push_back (i * 2 + 1);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i * 2);
    X.resize (20, 0);
    RA = range_it (A.begin ( ), A.end ( ));
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range_it (X.begin ( ), X.end ( ));

    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    BOOST_CHECK (RA.size ( ) == 10 and RD.size ( ) == 10);
    RX = full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 20);
    for (uint32_t i = 0; i < X.size ( ); ++i) BOOST_CHECK (X[i] == i);

    //----------------------------------------------------------------------
    // Three ranges
    //----------------------------------------------------------------------
    A.clear ( );
    D.clear ( );
    for (uint32_t i = 0; i < 10; ++i) A.push_back (i * 3 + 2);
    for (uint32_t i = 0; i < 10; ++i) B.push_back (i * 3 + 1);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i * 3);
    X.resize (30, 0);
    RA = range_it (A.begin ( ), A.end ( ));
    RB = range_it (B.begin ( ), B.end ( ));
    RC = range_it (C.begin ( ), C.end ( ));
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range_it (X.begin ( ), X.end ( ));
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    RX = full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 30);
    for (uint32_t i = 0; i < X.size ( ); ++i) BOOST_CHECK (X[i] == i);

    //---------------------------------------------------------------------
    // Four ranges
    //---------------------------------------------------------------------
    A.clear ( );
    B.clear ( );
    C.clear ( );
    D.clear ( );
    X.clear ( );
    for (uint32_t i = 0; i < 10; ++i) A.push_back (i * 4 + 3);
    for (uint32_t i = 0; i < 10; ++i) B.push_back (i * 4 + 2);
    for (uint32_t i = 0; i < 10; ++i) C.push_back (i * 4 + 1);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i * 4);

    X.resize (40, 0);
    RA = range_it (A.begin ( ), A.end ( ));
    RB = range_it (B.begin ( ), B.end ( ));
    RC = range_it (C.begin ( ), C.end ( ));
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range_it (X.begin ( ), X.end ( ));
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    RX = full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 40);
    for (uint32_t i = 0; i < X.size ( ); ++i) BOOST_CHECK (X[i] == i);

    //---------------------------------------------------------------------
    // Four ranges sorted
    //---------------------------------------------------------------------
    A.clear ( );
    B.clear ( );
    C.clear ( );
    D.clear ( );
    X.clear ( );
    for (uint32_t i = 0; i < 10; ++i) A.push_back (i);
    for (uint32_t i = 0; i < 10; ++i) B.push_back (i + 10);
    for (uint32_t i = 0; i < 10; ++i) C.push_back (i + 20);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i + 30);

    X.resize (40, 0);
    RA = range_it (A.begin ( ), A.end ( ));
    RB = range_it (B.begin ( ), B.end ( ));
    RC = range_it (C.begin ( ), C.end ( ));
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range_it (X.begin ( ), X.end ( ));
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    RX = full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 40);
    for (uint32_t i = 0; i < X.size ( ); ++i) BOOST_CHECK (X[i] == i);

    //---------------------------------------------------------------------
    // Four ranges sorted
    //---------------------------------------------------------------------
    A.clear ( );
    B.clear ( );
    C.clear ( );
    D.clear ( );
    X.clear ( );
    for (uint32_t i = 0; i < 10; ++i) A.push_back (i + 30);
    for (uint32_t i = 0; i < 10; ++i) B.push_back (i + 20);
    for (uint32_t i = 0; i < 10; ++i) C.push_back (i + 10);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i);

    X.resize (40, 0);
    RA = range_it (A.begin ( ), A.end ( ));
    RB = range_it (B.begin ( ), B.end ( ));
    RC = range_it (C.begin ( ), C.end ( ));
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range_it (X.begin ( ), X.end ( ));
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    RX = full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 40);
    for (uint32_t i = 0; i < X.size ( ); ++i) BOOST_CHECK (X[i] == i);
};

void test2 (void)
{
    typedef typename std::vector< xk >::iterator iter_t;
    typedef range< iter_t > range_it;
    typedef std::less< xk > compare;

    std::vector< xk > A, B, C, D, X;
    range_it R[4];
    compare comp;

    for (uint32_t i = 0; i < 10; ++i) {
        A.emplace_back (i, 0);
        B.emplace_back (i, 1);
        C.emplace_back (i, 2);
        D.emplace_back (i, 3);
    };
    X.resize (40);
    range_it RA (A.begin ( ), A.end ( ));
    range_it RB (B.begin ( ), B.end ( ));
    range_it RC (C.begin ( ), C.end ( ));
    range_it RD (D.begin ( ), D.end ( ));
    range_it RX (X.begin ( ), X.end ( ));

    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    RX = full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 40);
    for (uint32_t i = 0; i < X.size ( ); ++i)
        BOOST_CHECK (X[i].num == i / 4 and X[i].tail == i % 4);
}

void test3 (void)
{
    typedef typename std::vector< uint64_t >::iterator iter_t;
    typedef range< iter_t > range_it;
    typedef std::less< uint64_t > compare;

    uint64_t X[40];
    for (int i = 0; i < 40; ++i) X[i] = 0;
    std::vector< uint64_t > A, B, C, D;
    range_it R[4];
    compare comp;
    range_it RA (A.begin ( ), A.end ( ));
    range_it RB (B.begin ( ), B.end ( ));
    range_it RC (C.begin ( ), C.end ( ));
    range_it RD (D.begin ( ), D.end ( ));
    range< uint64_t * > RX (&X[0], &X[39]);

    //----------------------------------------------------------------------
    //   0 ranges
    //----------------------------------------------------------------------
    RX = uninit_full_merge4 (RX, R, 0, comp);
    BOOST_CHECK (RX.size ( ) == 0);

    //---------------------------------------------------------------------
    // 4 empty ranges
    //---------------------------------------------------------------------
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    BOOST_CHECK (RA.size ( ) == 0 and RB.size ( ) == 0 and RC.size ( ) == 0 and
                 RD.size ( ) == 0);
    RX = uninit_full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 0);

    //---------------------------------------------------------------------
    // 1 range filled and 3 empty
    //---------------------------------------------------------------------
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i);
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range< uint64_t * > (&X[0], &X[9]);
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    BOOST_CHECK (RD.size ( ) == 10);
    RX = uninit_full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 10);
    for (uint32_t i = 0; i < RX.size ( ); ++i) BOOST_CHECK (X[i] == i);

    //---------------------------------------------------------------------
    // Two ranges
    //---------------------------------------------------------------------
    D.clear ( );
    for (uint32_t i = 0; i < 10; ++i) A.push_back (i * 2 + 1);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i * 2);
    RA = range_it (A.begin ( ), A.end ( ));
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range< uint64_t * > (&X[0], &X[19]);

    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    BOOST_CHECK (RA.size ( ) == 10 and RD.size ( ) == 10);
    RX = uninit_full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 20);
    for (uint32_t i = 0; i < RX.size ( ); ++i) BOOST_CHECK (X[i] == i);

    //----------------------------------------------------------------------
    // Three ranges
    //----------------------------------------------------------------------
    A.clear ( );
    D.clear ( );
    for (uint32_t i = 0; i < 10; ++i) A.push_back (i * 3 + 2);
    for (uint32_t i = 0; i < 10; ++i) B.push_back (i * 3 + 1);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i * 3);
    RA = range_it (A.begin ( ), A.end ( ));
    RB = range_it (B.begin ( ), B.end ( ));
    RC = range_it (C.begin ( ), C.end ( ));
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range< uint64_t * > (&X[0], &X[29]);
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    RX = uninit_full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 30);
    for (uint32_t i = 0; i < RX.size ( ); ++i) BOOST_CHECK (X[i] == i);

    //---------------------------------------------------------------------
    // Four ranges
    //---------------------------------------------------------------------
    A.clear ( );
    B.clear ( );
    C.clear ( );
    D.clear ( );

    for (uint32_t i = 0; i < 10; ++i) A.push_back (i * 4 + 3);
    for (uint32_t i = 0; i < 10; ++i) B.push_back (i * 4 + 2);
    for (uint32_t i = 0; i < 10; ++i) C.push_back (i * 4 + 1);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i * 4);

    RA = range_it (A.begin ( ), A.end ( ));
    RB = range_it (B.begin ( ), B.end ( ));
    RC = range_it (C.begin ( ), C.end ( ));
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range< uint64_t * > (&X[0], &X[39]);
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    RX = uninit_full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 40);
    for (uint32_t i = 0; i < RX.size ( ); ++i) BOOST_CHECK (X[i] == i);

    //---------------------------------------------------------------------
    // Four ranges sorted
    //---------------------------------------------------------------------
    A.clear ( );
    B.clear ( );
    C.clear ( );
    D.clear ( );
    for (uint32_t i = 0; i < 10; ++i) A.push_back (i);
    for (uint32_t i = 0; i < 10; ++i) B.push_back (i + 10);
    for (uint32_t i = 0; i < 10; ++i) C.push_back (i + 20);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i + 30);

    RA = range_it (A.begin ( ), A.end ( ));
    RB = range_it (B.begin ( ), B.end ( ));
    RC = range_it (C.begin ( ), C.end ( ));
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range< uint64_t * > (&X[0], &X[39]);
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    RX = uninit_full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 40);
    for (uint32_t i = 0; i < RX.size ( ); ++i) BOOST_CHECK (X[i] == i);

    //---------------------------------------------------------------------
    // Four ranges sorted
    //---------------------------------------------------------------------
    A.clear ( );
    B.clear ( );
    C.clear ( );
    D.clear ( );
    for (uint32_t i = 0; i < 10; ++i) A.push_back (i + 30);
    for (uint32_t i = 0; i < 10; ++i) B.push_back (i + 20);
    for (uint32_t i = 0; i < 10; ++i) C.push_back (i + 10);
    for (uint32_t i = 0; i < 10; ++i) D.push_back (i);

    RA = range_it (A.begin ( ), A.end ( ));
    RB = range_it (B.begin ( ), B.end ( ));
    RC = range_it (C.begin ( ), C.end ( ));
    RD = range_it (D.begin ( ), D.end ( ));
    RX = range< uint64_t * > (&X[0], &X[39]);
    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    RX = uninit_full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 40);
    for (uint32_t i = 0; i < RX.size ( ); ++i) BOOST_CHECK (X[i] == i);
};

void test4 (void)
{
    typedef typename std::vector< xk >::iterator iter_t;
    typedef range< iter_t > range_it;
    typedef std::less< xk > compare;

    xk X[40];
    std::vector< xk > A, B, C, D;
    range_it R[4];
    compare comp;

    for (uint32_t i = 0; i < 10; ++i) {
        A.emplace_back (i, 0);
        B.emplace_back (i, 1);
        C.emplace_back (i, 2);
        D.emplace_back (i, 3);
    };
    for (int i = 0; i < 40; ++i) X[i] = 0;
    range_it RA (A.begin ( ), A.end ( ));
    range_it RB (B.begin ( ), B.end ( ));
    range_it RC (C.begin ( ), C.end ( ));
    range_it RD (D.begin ( ), D.end ( ));
    range< xk * > RX (&X[0], &X[39]);

    R[0] = RA;
    R[1] = RB;
    R[2] = RC;
    R[3] = RD;
    RX = uninit_full_merge4 (RX, R, 4, comp);
    BOOST_CHECK (RX.size ( ) == 40);
    for (uint32_t i = 0; i < RX.size ( ); ++i)
        BOOST_CHECK (X[i].num == i / 4 and X[i].tail == i % 4);
}

int test_main (int, char *[])
{
    test1 ( );
    test2 ( );
    test3 ( );
    test4 ( );
    return 0;
};
