//----------------------------------------------------------------------------
/// @file test_algorithms.cpp
/// @brief Test program of several routines containes in the files
///        algorithms32.hpp and algorithms64.hpp
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <boost/sort/parallel/util/algorithm.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#define NELEM 1000000



using namespace boost::sort::parallel::util;

void prueba32 ( void)
{   //------------------------------- begin ----------------------------

    //------------------------------------------------------------------
    // Test LS1B_32 ( Least Significant 1 Bit )
    // Test MS1B_32 ( Most  Significant 1 Bit )
    //-----------------------------------------------------------------
    BOOST_CHECK ( LS1B_32 (0x5324) == 2 );
    BOOST_CHECK ( LS1B_32 (0x5320U) == 5 );
    BOOST_CHECK ( LS1B_32 (0x80000000U) == 31);
    BOOST_CHECK ( LS1B_32 (0x80001000U) == 12);


    BOOST_CHECK ( MS1B_32 (0x5324) == 14);
    BOOST_CHECK ( MS1B_32 (0x80000000U) == 31);
    BOOST_CHECK ( MS1B_32 (1U) == 0);

    //--------------------------------------------------------------------
    // NBits
    //--------------------------------------------------------------------
    BOOST_CHECK ( NBits32 (63) ==6);
    BOOST_CHECK ( NBits32 (64) ==7);
    BOOST_CHECK ( NBits32 (65) ==7);
    BOOST_CHECK ( NBits (63) ==6);
    BOOST_CHECK ( NBits (64) ==7);
    BOOST_CHECK ( NBits (65) ==7);

    //------------------------------------------------------------------
    // Test LS1B_64 ( Least Significant 1 Bit )
    // Test MS1B_64 ( Most  Significant 1 Bit )
    //-----------------------------------------------------------------

    BOOST_CHECK ( MS1B_32 (0x5324) == 14);
    BOOST_CHECK ( MS1B_32 (0x80000000U) == 31);
    BOOST_CHECK ( MS1B_32 (1U) == 0);

    //std::cout<<BitScanReverse32 (0x5324) <<std::endl ;
    BOOST_CHECK ( BitScanReverse32 (0x5324) == 14);
    BOOST_CHECK ( BitScanReverse32 (0x80000000U) == 31);
    BOOST_CHECK ( BitScanReverse32 (1U) == 0);

};
void prueba64 ( void)
{   //------------------------------- begin ----------------------------

    //------------------------------------------------------------------
    // Test LS1B_64 ( Least Significant 1 Bit )
    // Test MS1B_64 ( Most  Significant 1 Bit )
    //-----------------------------------------------------------------
    BOOST_CHECK ( LS1B_64 (0x5324ULL) == 2 );
    BOOST_CHECK ( LS1B_64 (0x5320ULL) == 5 );
    BOOST_CHECK ( LS1B_64 (0x80000000ULL) == 31);
    BOOST_CHECK ( LS1B_64 (0x80001000ULL) == 12);

    BOOST_CHECK ( MS1B_64 (0x5324) == 14);
    BOOST_CHECK ( MS1B_64 (0x80000000U) == 31);
    BOOST_CHECK ( MS1B_64 (1U) == 0);

    //--------------------------------------------------------------------
    // NBits
    //--------------------------------------------------------------------
    BOOST_CHECK ( NBits64 (63) ==6);
    BOOST_CHECK ( NBits64 (64) ==7);
    BOOST_CHECK ( NBits64 (65) ==7);
    BOOST_CHECK ( NBits (63) ==6);
    BOOST_CHECK ( NBits (64) ==7);
    BOOST_CHECK ( NBits (65) ==7);

};

int test_main( int, char*[] )
{
    prueba32 ();
    prueba64 ();

    return 0;
};
