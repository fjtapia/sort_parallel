//----------------------------------------------------------------------------
/// @file test_buffer.cpp
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
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
//#include <iostream>
#include <boost/sort/parallel/util/buffer.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <vector>

#include <algorithm>

namespace bs_util = boost::sort::parallel::util ;

void prueba1 ();
void prueba2 ();
void prueba3 ();
void prueba4 ();
void prueba5 ();
void prueba6 ();
//
//****************************************************************************
//  TEST MOVE, CONSTRUCT, UNINITIALIZED_MOVE, DESTROY
//****************************************************************************
void prueba1 ( void )
{   //-------------------- test of move --------------------------------------
    std::vector<uint64_t> A , B;

    A.resize ( 10, 0);
    B.resize ( 10, 0);
    for ( uint32_t i =0 ; i < 10 ; ++i) A[i] = i ;

    bs_util::move_buf(  &B[0], A.begin() , A.end());
    for ( uint32_t i =0 ; i < 10 ; ++i)
        BOOST_CHECK ( B[i] == i) ;

    //------------- test of construct , destroy and uninitialized_move -------
    struct forensic
    {   int64_t N ;
        forensic ( uint64_t K = 0 ){ N = (int64_t)K ;};
        ~forensic () { N = -1 ;};
    };
    char K[80];
    forensic *PAux = reinterpret_cast <forensic*> ( &K[0]);

    for ( uint32_t i =0 ; i < 10 ; ++i)
        bs_util::construct( PAux+i, i);
    for ( uint32_t i = 0 ; i < 10 ; ++i)
    {   BOOST_CHECK ( PAux[i].N == i);
    };
    bs_util::destroy ( PAux , PAux+10);

    //----------------test of uninitialized_move---------------------------
    std::vector <forensic> V ;
    for ( uint32_t i =0 ; i < 10 ; ++i) V.emplace_back ( i);

    bs_util::uninit_move_buf ( PAux, V.begin() , V.end());
    for ( uint32_t i =0 ; i < 10 ; ++i)
        BOOST_CHECK ( PAux[i].N == i) ;

    bs_util::destroy ( PAux , PAux+10);


};
//
//****************************************************************************
//      TEST OF FULL_MERGE
//****************************************************************************
void prueba2 ()
{   //---------------------------- begin -------------------------------------
    typedef std::less<uint64_t> compare ;
    std::vector<uint64_t> A , B;
    //uint64_t A[20],B[20];
    size_t NA =0 ;

    A.clear() ;
    B.clear() ;
    B.assign ( 20 , 0 );
    for ( uint32_t i =0  ; i < 20 ; i+=2) A.push_back( i);
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.push_back( i);

    bs_util::full_merge ( &A[0], &A[10], &A[10], &A[20], &B[0], compare() )  ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( B[i]== i);

    //NA =0 ;
    for ( uint32_t i =0 ; i < 20 ; ++i)B[i] = 100 ;
    for ( uint32_t i =0  ; i < 20 ; i++) A[i] = i;

    bs_util::full_merge ( &A[0], &A[10], &A[10], &A[20], &B[0], compare() )  ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( B[i] == i) ;

    NA  = 0 ;
    for ( uint32_t i =0 ; i < 20 ; ++i)B[i] = 100 ;
    for ( uint32_t i =0  ; i < 10 ; i++) A[NA++] = 10+i;
    for ( uint32_t i =0  ; i < 10 ; i++) A[NA++] = i;
    bs_util::full_merge ( &A[0], &A[10], &A[10], &A[20], &B[0], compare() )  ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( B[i]== i);

};
//
//****************************************************************************
//      TEST OF HALF_MERGE
//****************************************************************************
void prueba3 ()
{   //---------------------------- begin -------------------------------------
    typedef std::less<uint64_t> compare ;
    std::vector<uint64_t> A , B;
    //uint64_t A[20],B[20];

    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 20 ; i+=2) B.push_back( i);
    A.resize (10,0);
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.push_back( i);
    bs_util::half_merge (&B[0] , &B[10], &A[10] , &A[20], &A[0], compare());
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( A[i] == i) ;

    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0 ; i < 10 ; ++i)B.push_back(i );
    A.resize (10,0);
    for ( uint32_t i =10  ; i < 20 ; i++) A.push_back( i);
    bs_util::half_merge (&B[0] , &B[10], &A[10] , &A[20], &A[0] , compare());
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( A[i]== i);

    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 10 ; i++) B.push_back( 10+i);
    A.resize (10,0);
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back( i);
    bs_util::half_merge (&B[0] , &B[10], &A[10] , &A[20], &A[0], compare()) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( A[i]== i) ;

};

//
//****************************************************************************
//      TEST OF UNINITIALIZED_FULL_MERGE
//****************************************************************************
void prueba5 ()
{   //---------------------------- begin -------------------------------------
    struct forensic
    {   int64_t N ;
        forensic ( uint64_t K = 0 ){ N = (int64_t)K ;};
        ~forensic () { N = -1 ;};
        bool operator < ( const forensic & f)const{ return (N < f.N); };
    };
    char K[1600];
    forensic *PAux = reinterpret_cast <forensic*> ( &K[0]);

    typedef std::less<forensic > compare ;
    std::vector<forensic> A ;

    A.clear() ;
    for ( uint32_t i =0  ; i < 20 ; i+=2) A.emplace_back( i);
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.emplace_back( i);

    bs_util::uninit_full_merge ( &A[0], &A[10], &A[10], &A[20],
                                        PAux , compare() );
    for ( uint32_t i = 0  ; i < 20 ; ++i)
    {   BOOST_CHECK ( PAux[i].N == i);
    };
    bs_util::destroy ( PAux, PAux + 20);

    //NA =0 ;
    for ( uint32_t i =0  ; i < 20 ; i++) A[i] = i;

    bs_util::uninit_full_merge ( &A[0], &A[10], &A[10], &A[20],
                                        PAux  , compare() ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
    {   BOOST_CHECK ( PAux[i]. N == i);
    };
    bs_util::destroy ( PAux, PAux + 20);

    for ( uint32_t i =0  ; i < 10 ; i++) A[i] = 10+i;
    for ( uint32_t i =0  ; i < 10 ; i++) A[10+i] = i;
    bs_util::uninit_full_merge ( &A[0], &A[10], &A[10],&A[20],
                                        PAux  , compare() );
    for ( uint32_t i = 0  ; i < 20 ; ++i)
    {   BOOST_CHECK ( PAux[i]. N == i );
    };
    bs_util::destroy ( PAux, PAux + 20);
};

int test_main( int, char*[] )
{   //-------------- begin------------
    prueba1() ;
    prueba2() ;
    prueba3() ;
    prueba5() ;
    return 0 ;

};
