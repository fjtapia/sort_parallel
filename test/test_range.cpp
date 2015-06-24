//----------------------------------------------------------------------------
/// @file test_range.cpp
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
#include <iostream>
#include <boost/sort/parallel/util/range.hpp>
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
void prueba7 ();
void prueba8 ();
void prueba9 ();
void prueba10 ();
void prueba11 ();
void prueba12 ();
void prueba13 ();
void prueba14 ();
//
//****************************************************************************
//          TEST MOVE, UNINITIALIZED_MOVE, DESTROY
//****************************************************************************
void prueba1 ( void )
{   //-----------------------------------------------------------------------
    //              test of move
    //------------------------------------------------------------------------
    typedef typename std::vector<uint64_t>::iterator iter_t ;
    typedef bs_util::range<iter_t>  range_t ;

    std::vector<uint64_t> A , B;

    A.resize ( 10, 0);
    B.resize ( 10, 0);
    for ( uint32_t i =0 ; i < 10 ; ++i) A[i] = i ;

    bs_util::move( &B[0], range_t(A.begin() , A.end()));
    for ( uint32_t i =0 ; i < 10 ; ++i)
        BOOST_CHECK ( B[i] == i) ;

    //------------------------------------------------------------------------
    //           test of uninitialized_move , destroy
    //------------------------------------------------------------------------
    struct forensic
    {   int64_t N ;
        forensic ( uint64_t K = 0 ){ N = (int64_t)K ;};
        ~forensic () { N = -1 ;};
    };
    typedef typename std::vector<forensic>::iterator fiter_t ;
    typedef bs_util::range<fiter_t>  frange_t ;

    char K[160];
    forensic *PAux = reinterpret_cast <forensic*> ( &K[0]);

    std::vector <forensic> V ;
    for ( uint32_t i =0 ; i < 10 ; ++i) V.emplace_back ( i);

    bs_util::uninit_move ( PAux, frange_t(V.begin(),V.end()));
    for ( uint32_t i =0 ; i < 10 ; ++i)
        BOOST_CHECK ( PAux[i].N == i) ;

    bs_util::destroy ( bs_util::range <forensic*> (PAux , PAux+10));
    //for ( uint32_t i =0 ; i < 10 ; ++i)
    //{   if ( PAux[i].N != -1)
    //        std::cout<<"Error in the destroy function \n";
    //};
};
//
//****************************************************************************
//                    TEST OF FULL_MERGE
//****************************************************************************
void prueba2 ()
{   //---------------------------- begin -------------------------------------
    typedef std::less<uint64_t>  compare ;
    typedef bs_util::range<uint64_t *> rng ;

    std::vector<uint64_t> A , B;
    compare comp ;

    A.clear() ;
    B.clear() ;
    B.assign ( 20 , 0 );
    for ( uint32_t i =0  ; i < 20 ; i+=2) A.push_back( i);
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.push_back( i);

    bs_util::full_merge ( &B[0], rng(&A[0],&A[10]),rng(&A[10],&A[20]), comp )  ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( B[i] == i);

    A.clear() ;
    B.clear() ;
    B.assign ( 20 , 100 );
    for ( uint32_t i =0  ; i < 20 ; i++) A.push_back (i);

    bs_util::full_merge ( &B[0],rng( &A[0],&A[10]) ,rng(&A[10],&A[20]),comp)  ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
    {   BOOST_CHECK ( B[i] == i);
    };

    A.clear() ;
    B.clear() ;
    B.assign ( 20 , 100 );

    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back (10+i);
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back(i);
    bs_util::full_merge ( &B[0],rng( &A[0],&A[10]),rng(&A[10],&A[20]),comp );
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( B[i] == i) ;
};
//
//****************************************************************************
//      TEST OF UNINITIALIZED_FULL_MERGE
//****************************************************************************
void prueba3 ()
{   //---------------------------- begin -------------------------------------
    struct forensic
    {   int64_t N ;
        forensic ( uint64_t K = 0 ){ N = (int64_t)K ;};
        ~forensic () { N = -1 ;};
        bool operator < ( const forensic & f)const{ return (N < f.N); };
    };
    typedef bs_util::range<forensic*> range_t ;

    char K[1600];
    forensic *PAux = reinterpret_cast <forensic*> ( &K[0]);

    typedef std::less<forensic > compare ;
    std::vector<forensic> A ;

    A.clear() ;
    for ( uint32_t i =0  ; i < 20 ; i+=2) A.emplace_back( i);
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.emplace_back( i);

    bs_util::uninit_full_merge ( PAux ,range_t(&A[0],&A[10]),
                                        range_t(&A[10],&A[20]),compare() );
    for ( uint32_t i = 0  ; i < 20 ; ++i)
    {   BOOST_CHECK ( PAux[i].N == i);
    };
    bs_util::destroy ( PAux, PAux + 20);

    //NA =0 ;
    for ( uint32_t i =0  ; i < 20 ; i++) A[i] = i;

    bs_util::uninit_full_merge ( PAux  ,range_t(&A[0],&A[10]),
                                        range_t(&A[10], &A[20]),compare() ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
    {   if ( PAux[i]. N != i)
            std::cout<<"error "<<i<<"\n";
    };
    bs_util::destroy ( PAux, PAux + 20);

    for ( uint32_t i =0  ; i < 10 ; i++) A[i] = 10+i;
    for ( uint32_t i =0  ; i < 10 ; i++) A[10+i] = i;
    bs_util::uninit_full_merge ( PAux, range_t(&A[0], &A[10]),
                                        range_t(&A[10],&A[20]),compare() );
    for ( uint32_t i = 0  ; i < 20 ; ++i)
    {   if ( PAux[i]. N != i )
            std::cout<<"error "<<i<<"\n";
    };
    bs_util::destroy ( range_t (PAux, PAux + 20));

};
//
//****************************************************************************
//                          TEST OF HALF_MERGE
//****************************************************************************
void prueba4 ()
{   //-------------------------- begin ---------------------------------------
    typedef bs_util::range<uint64_t *> rng ;
    typedef std::less<uint64_t>  compare ;

    compare   comp ;
    std::vector<uint64_t> A , B;

    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 20 ; i+=2) B.push_back( i);
    A.resize (10,0);
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.push_back( i);

    bs_util::half_merge (&A[0],rng(&B[0],&B[10]), rng(&A[10],&A[20]),comp);
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( A[i] == i) ;

    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0 ; i < 10 ; ++i)B.push_back(i );
    A.resize (10,0);
    for ( uint32_t i =10  ; i < 20 ; i++) A.push_back( i);

    bs_util::half_merge (&A[0],rng(&B[0],&B[10]),rng(&A[10],&A[20]),comp);
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( A[i] == i);

    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 10 ; i++) B.push_back( 10+i);
    A.resize (10,0);
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back( i);
    bs_util::half_merge (&A[0],rng(&B[0],&B[10]), rng(&A[10],&A[20]),comp);
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( A[i] == i);
};

//
//****************************************************************************
//               TEST OF LESS_RANGE AND FULL_MERGE_FOUR
//****************************************************************************
void prueba5 ( void)
{   //---------------------------------- begin -------------------------------
    typedef std::less<uint64_t>  compare ;
    compare   comp ;
    typedef bs_util::range<uint64_t *> rng ;

    //------------------------------------------------------------------------
    uint64_t N1 = 3, N2 = 2 ;

    assert ( bs_util::less_range ( &N1,2, &N2,1,comp) == false);
    assert ( bs_util::less_range ( &N1,2, &N1,1,comp) == false);
    assert ( bs_util::less_range ( &N1,1, &N1,2,comp) == true );
    assert ( bs_util::less_range ( &N2,2, &N1,0,comp) == true );

    //------------------------------------------------------------------------
    rng R[4];
    uint64_t X0[10], X1[10],X2[10],X3[10], Y[40];

    //------------------------------------------------------------------------
    for ( uint32_t i =0 ; i < 10 ; ++i ) X0[i] = 0+ i*4 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X1[i] = 1+ i*4 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X2[i] = 2+ i*4 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X3[i] = 3+ i*4 ;
    R[0]= bs_util::range<uint64_t*> ( &X0[0], &X0[10]);
    R[1]= bs_util::range<uint64_t*> ( &X1[0], &X1[10]);
    R[2]= bs_util::range<uint64_t*> ( &X2[0], &X2[10]);
    R[3]= bs_util::range<uint64_t*> ( &X3[0], &X3[10]);
    for ( uint32_t i =0 ; i < 40 ; ++i) Y[i] = 100 ;
    bs_util::full_merge4 ( &Y[0], R , 4 , comp);
    for ( uint32_t i =0 ; i < 40 ; ++i)
        BOOST_CHECK ( Y[i] == i );

    //------------------------------------------------------------------------
    for ( uint32_t i =0 ; i < 10 ; ++i ) X0[i] = 0+ i*3 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X1[i] = 1+ i*3 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X2[i] = 2+ i*3 ;
    R[0]= bs_util::range<uint64_t*> ( &X0[0], &X0[10]);
    R[1]= bs_util::range<uint64_t*> ( &X1[0], &X1[10]);
    R[2]= bs_util::range<uint64_t*> ( &X2[0], &X2[10]);
    for ( uint32_t i =0 ; i < 40 ; ++i) Y[i] = 100 ;
    bs_util::full_merge4 ( &Y[0], R , 3 , comp);
    for ( uint32_t i =0 ; i < 30 ; ++i)
        BOOST_CHECK ( Y[i] == i ) ;

    //------------------------------------------------------------------------
    for ( uint32_t i =0 ; i < 10 ; ++i ) X0[i] = 0+ i*2 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X1[i] = 1+ i*2 ;
    R[0]= bs_util::range<uint64_t*> ( &X0[0], &X0[10]);
    R[1]= bs_util::range<uint64_t*> ( &X1[0], &X1[10]);
    for ( uint32_t i =0 ; i < 40 ; ++i) Y[i] = 100 ;
    bs_util::full_merge4 ( &Y[0], R , 2 , comp);
    for ( uint32_t i =0 ; i < 20 ; ++i)
        BOOST_CHECK ( Y[i] == i ) ;

    //------------------------------------------------------------------------
    for ( uint32_t i =0 ; i < 10 ; ++i ) X0[i] = i;
    R[0]= bs_util::range<uint64_t*> ( &X0[0], &X0[10]);
    for ( uint32_t i =0 ; i < 40 ; ++i) Y[i] = 100 ;
    bs_util::full_merge4 ( &Y[0], R , 1 , comp);
    for ( uint32_t i =0 ; i < 10 ; ++i)
        BOOST_CHECK ( Y[i] == i ) ;

};
//
//****************************************************************************
//               TEST OF FULL_MERGE_FOUR
//****************************************************************************
void prueba6(void)
{   //------------------------------- begin------------------------------------
    struct xk
    {   unsigned tail : 3 ;
        unsigned num  :24 ;

        xk( unsigned N, unsigned T =0):tail(T),num(N){};
        bool operator < ( xk A) const
        {   return ( num < A.num);
        };
    };
    std::vector<xk> R[4],Rout;
    typedef typename std::vector<xk>::iterator iter_t ;
    typedef  std::less<xk> Compare ;
    Compare Comp ;

    //------------------------------------------------------------------------
    R[0] = {3,5,9,16,17,24,25,29};
    R[1] = {1,7,10,13,18,22,26,30 };
    R[2] = {2 ,6,11,15,20,21,28,32};
    R[3] = {4,8,12,14,19,23,27,31} ;

    for ( uint32_t i =0 ; i < 4 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
        {   R[i][k].tail = i ;
        };
    };
    Rout.resize ( 32,0);
    bs_util::range<iter_t>  RG[4]= { {R[0].begin(),R[0].end()} ,
                            {R[1].begin(),R[1].end()} ,
                            {R[2].begin(),R[2].end()} ,
                            {R[3].begin(),R[3].end()}  } ;

    bs_util::full_merge4 (&Rout[0],RG,4,Comp);

    if ( Rout.size() != 32) std::cout<<"Error in the size\n";
    for( uint32_t i = 0 ; i < Rout.size() ; ++i)
        BOOST_CHECK ( Rout[i].num == i+1) ;

    //------------------------------------------------------------------------
    R[0] = {2,3,5,6,6,10,13,20};
    R[1] = {1,2,4,5,6,10,17,19 };
    R[2] = {2,3,4,7,8,11,16,18};
    R[3] = {1,4,5,7,9,12,14,15} ;
    for ( uint32_t i =0 ; i < 4 ; ++i)
    {   for ( uint32_t k =0 ; k < R[i].size() ; ++k)
            R[i][k].tail = i ;
    };
    Rout.clear() ;
    Rout.resize ( 32,0);
    RG[0] = {R[0].begin(),R[0].end()} ;
    RG[1] = {R[1].begin(),R[1].end()} ;
    RG[2] =  {R[2].begin(),R[2].end()} ;
    RG[3] =  {R[3].begin(),R[3].end()}   ;

    bs_util::full_merge4 (&Rout[0],RG,4,Comp);

    for ( uint32_t i = 1 ; i < Rout.size() ; ++i)
    {
        BOOST_CHECK ( Rout[i-1].num <= Rout[i].num and
             (Rout[i-1].num != Rout[i].num or Rout[i-1].tail <= Rout[i].tail  ));
        //if ( Rout[i-1].num > Rout[i].num or
        //     (Rout[i-1].num == Rout[i].num and Rout[i-1].tail> Rout[i].tail  ))
        //    std::cout<<"Error in the sorting \n" ;
    };
};

//
//****************************************************************************
//               TEST OF UNINITIALIZED_FULL_MERGE_FOUR
//****************************************************************************
void prueba7 ( void)
{   //---------------------------------- begin -------------------------------
    typedef std::less<uint64_t>  compare ;
    compare   comp ;
    typedef bs_util::range<uint64_t *> rng ;

    //------------------------------------------------------------------------
    rng R[4];
    uint64_t X0[10], X1[10],X2[10],X3[10];
    char K[320];
    uint64_t *PAux = reinterpret_cast <uint64_t*> ( &K[0]);

    //------------------------------------------------------------------------
    for ( uint32_t i =0 ; i < 10 ; ++i ) X0[i] = 0+ i*4 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X1[i] = 1+ i*4 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X2[i] = 2+ i*4 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X3[i] = 3+ i*4 ;
    R[0]= bs_util::range<uint64_t*> ( &X0[0], &X0[10]);
    R[1]= bs_util::range<uint64_t*> ( &X1[0], &X1[10]);
    R[2]= bs_util::range<uint64_t*> ( &X2[0], &X2[10]);
    R[3]= bs_util::range<uint64_t*> ( &X3[0], &X3[10]);

    bs_util::uninit_full_merge4 ( PAux, R , 4 , comp);
    for ( uint32_t i =0 ; i < 40 ; ++i)
        BOOST_CHECK ( PAux[i] == i ) ;
    bs_util::destroy (PAux, PAux + 40 );

    //------------------------------------------------------------------------
    for ( uint32_t i =0 ; i < 10 ; ++i ) X0[i] = 0+ i*3 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X1[i] = 1+ i*3 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X2[i] = 2+ i*3 ;
    R[0]= bs_util::range<uint64_t*> ( &X0[0], &X0[10]);
    R[1]= bs_util::range<uint64_t*> ( &X1[0], &X1[10]);
    R[2]= bs_util::range<uint64_t*> ( &X2[0], &X2[10]);

    bs_util::uninit_full_merge4 ( PAux, R , 3 , comp);
    for ( uint32_t i =0 ; i < 30 ; ++i)
        BOOST_CHECK ( PAux[i] == i ) ;
    bs_util::destroy (PAux, PAux + 40 );

    //------------------------------------------------------------------------
    for ( uint32_t i =0 ; i < 10 ; ++i ) X0[i] = 0+ i*2 ;
    for ( uint32_t i =0 ; i < 10 ; ++i ) X1[i] = 1+ i*2 ;
    R[0]= bs_util::range<uint64_t*> ( &X0[0], &X0[10]);
    R[1]= bs_util::range<uint64_t*> ( &X1[0], &X1[10]);
    bs_util::uninit_full_merge4 ( PAux, R , 2 , comp);
    for ( uint32_t i =0 ; i < 20 ; ++i)
        BOOST_CHECK ( PAux[i] == i ) ;
    bs_util::destroy (PAux, PAux + 40 );

    //------------------------------------------------------------------------
    for ( uint32_t i =0 ; i < 10 ; ++i ) X0[i] = i;
    R[0]= bs_util::range<uint64_t*> ( &X0[0], &X0[10]);

    bs_util::uninit_full_merge4 ( PAux, R , 1 , comp);
    for ( uint32_t i =0 ; i < 10 ; ++i)
        BOOST_CHECK ( PAux[i] == i ) ;
    bs_util::destroy (PAux, PAux + 40 );
};

//****************************************************************************
//               TEST OF UNINITIALIZED_FULL_MERGE_FOUR
//****************************************************************************
void prueba8(void)
{   //------------------------------- begin------------------------------------
    struct xk
    {   unsigned tail : 3 ;
        unsigned num  :24 ;

        xk( unsigned N, unsigned T =0):tail(T),num(N){};
        bool operator < ( xk A) const
        {   return ( num < A.num);
        };
    };
    typedef typename std::vector<xk>::iterator iter_t ;
    typedef  std::less<xk> Compare ;

    std::vector<xk> R[4];
    char K[320];
    xk *PAux = reinterpret_cast <xk*> ( &K[0]);

    Compare Comp ;

    //------------------------------------------------------------------------
    R[0] = {3,5,9,16,17,24,25,29};
    R[1] = {1,7,10,13,18,22,26,30 };
    R[2] = {2 ,6,11,15,20,21,28,32};
    R[3] = {4,8,12,14,19,23,27,31} ;

    for ( uint32_t i =0 ; i < 4 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
        {   R[i][k].tail = i ;
        };
    };

    bs_util::range<iter_t>  RG[4]= { {R[0].begin(),R[0].end()} ,
                            {R[1].begin(),R[1].end()} ,
                            {R[2].begin(),R[2].end()} ,
                            {R[3].begin(),R[3].end()}  } ;

    bs_util::uninit_full_merge4 (PAux,RG,4,Comp);
    for( uint32_t i = 0 ; i < 32 ; ++i)
        BOOST_CHECK ( PAux[i].num == i+1) ;
    bs_util::destroy (PAux, PAux + 32 );

    //------------------------------------------------------------------------
    R[0] = {2,3,5,6,6,10,13,20};
    R[1] = {1,2,4,5,6,10,17,19 };
    R[2] = {2,3,4,7,8,11,16,18};
    R[3] = {1,4,5,7,9,12,14,15} ;
    for ( uint32_t i =0 ; i < 4 ; ++i)
    {   for ( uint32_t k =0 ; k < R[i].size() ; ++k)
            R[i][k].tail = i ;
    };

    RG[0] = {R[0].begin(),R[0].end()} ;
    RG[1] = {R[1].begin(),R[1].end()} ;
    RG[2] =  {R[2].begin(),R[2].end()} ;
    RG[3] =  {R[3].begin(),R[3].end()}   ;

    bs_util::uninit_full_merge4 (PAux,RG,4,Comp);

    for ( uint32_t i = 1 ; i < 32 ; ++i)
    {   //if ( PAux[i-1].num > PAux[i].num or
        //     (PAux[i-1].num == PAux[i].num and PAux[i-1].tail> PAux[i].tail  ))
        //    std::cout<<"Error in the sorting \n" ;
        BOOST_CHECK ( PAux[i-1].num <= PAux[i].num and
             (PAux[i-1].num != PAux[i].num or PAux[i-1].tail <= PAux[i].tail  ));
    };
    bs_util::destroy (PAux, PAux + 32 );
};
//
//****************************************************************************
//               TEST OF  MERGE_LEVEL4
//****************************************************************************
void prueba9 ( void)
{   //------------------------------- begin-----------------------------------
    uint64_t X[10][10], Y[100];

    for ( uint32_t i =0 ; i < 4 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
            X[i][k] = i+ k*4 ;
    };
    for ( uint32_t i =0 ; i < 3 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
            X[i+4][k] = i+ 40+k*3 ;
    };
    for ( uint32_t i =0 ; i < 3 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
            X[i+7][k] = i+ 70+k*3 ;
    };

    for ( uint32_t i =0 ; i < 100 ; ++i) Y[i] = 1000 ;
    std::vector< bs_util::range <uint64_t*> > V , Z;
    for ( uint32_t i  =0 ; i < 10 ; ++i)
        V.emplace_back( &X[i][0], &X[i][10]);
    bs_util::merge_level4 ( &Y[0], V, Z , std::less<uint64_t> ());

    for ( uint32_t i = 0 ; i < 100 ; ++i)
        BOOST_CHECK ( Y[i] == i);
    BOOST_CHECK ( Z.size() == 3 ) ;
} ;
//
//****************************************************************************
//               TEST OF  UNINIT_MERGE_LEVEL4
//****************************************************************************
void prueba10 ( void)
{   //------------------------------- begin-----------------------------------
    uint64_t X[10][10];

    char K[800];
    uint64_t *PAux = reinterpret_cast <uint64_t*> ( &K[0]);

    for ( uint32_t i =0 ; i < 4 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
            X[i][k] = i+ k*4 ;
    };
    for ( uint32_t i =0 ; i < 3 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
            X[i+4][k] = i+ 40+k*3 ;
    };
    for ( uint32_t i =0 ; i < 3 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
            X[i+7][k] = i+ 70+k*3 ;
    };

    for ( uint32_t i =0 ; i < 800 ; ++i) K[i] = 0xff;
    std::vector< bs_util::range <uint64_t*> > V , Z;
    for ( uint32_t i  =0 ; i < 10 ; ++i)
        V.emplace_back( &X[i][0], &X[i][10]);
    bs_util::uninit_merge_level4 ( PAux, V, Z , std::less<uint64_t> ());

    for ( uint32_t i = 0 ; i < 100 ; ++i)
        BOOST_CHECK ( PAux[i] == i);
    BOOST_CHECK ( Z.size() == 3 ) ;
} ;
//
//****************************************************************************
//                 TEST OF MERGE_VECTOR4
//****************************************************************************
void prueba11 ( void)
{   //------------------------------- begin-----------------------------------
    uint64_t X[10][10], Y[100];

    for ( uint32_t i =0 ; i < 100 ; ++i) Y[i] = 99999 ;
    for ( uint32_t i =0 ; i < 4 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
            X[i][k] = i+60+ k*4 ;
    };
    for ( uint32_t i =0 ; i < 3 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
            X[i+4][k] = i+ +30+k*3 ;
    };
    for ( uint32_t i =0 ; i < 3 ; ++i)
    {   for ( uint32_t k =0 ; k < 10 ; ++k)
            X[i+7][k] = i+ k*3 ;
    };


    //for ( uint32_t i =0 ; i < 100 ; ++i) Y[i] = 1000 ;
    std::vector< bs_util::range <uint64_t*> > V , Z;
    for ( uint32_t i  =0 ; i < 10 ; ++i)
    {   V.emplace_back( &X[i][0], &X[i][10]);
        Z.emplace_back( &Y[10*i], &Y [10 * (i+1)]);
    };

    merge_vector4 ( & X[0][0],&Y[0], V, Z , std::less<uint64_t> ());
    for ( uint32_t i = 0 ; i < 100 ; ++i)
        BOOST_CHECK ( Y[i] == i);
};
//
//****************************************************************************
//                 TEST OF MERGE_VECTOR4
//****************************************************************************
void prueba12 ( void)
{   //------------------------------- begin-----------------------------------
    uint64_t X[1000][10], Y[10000];
    std::vector< bs_util::range <uint64_t*> > V , Z;

    for ( uint32_t Nrange = 0 ; Nrange < 1000 ; ++Nrange )
    {   for ( uint32_t k =0 ; k < Nrange ; ++k)
        {   for ( uint32_t i =0 ; i < 10 ; ++i)
            {   X[k][i]= (Nrange-k-1) *10 +i;
            };
        };

        for ( uint32_t i =0 ; i < 10 * Nrange ; ++i) Y[i] = 1000000 ;
        V.clear() ;
        for ( uint32_t i = 0 ; i < Nrange ; ++i)
        {   V.emplace_back( &X[i][0], &X[i][10]);
        };
        bs_util::merge_vector4 ( &X[0][0], &Y[0], V,Z,std::less<uint64_t> ());

        for ( uint32_t i = 0 ; i < Nrange * 10 ; ++i)
            BOOST_CHECK ( Y[i] == i) ;
    };
};


void prueba15 ()
{   //--------------------------- begin -------------------------------
    std::vector<uint64_t> A, B, C ( 30 , 0) ;

    for ( uint32_t i =0 ; i < 10 ; ++i)
    {   A.push_back ( 100+i);
        B.push_back ( 80 +i);
    };
    bs_util::range<uint64_t*> R1 ( &A[0], &A[10]), R2 ( &B[0], &B[10]);
    uninit_full_merge (&C[0],R1,R2, std::less<uint64_t>() );
    for ( uint32_t i =0 ; i < 10 ; ++i)
        BOOST_CHECK ( C[i] == 80+i);
    for ( uint32_t i =10 ; i < 20 ; ++i)
        BOOST_CHECK ( C[i] == 90+i) ;


}
int test_main( int, char*[] )
{   //-------------- begin------------
    prueba1() ;
    prueba2() ;
    prueba3() ;
    prueba4() ;
    prueba5() ;
    prueba6() ;
    prueba7() ;
    prueba8() ;
    prueba9() ;
    prueba10() ;
    prueba11() ;
    prueba12() ;
    prueba15() ;
    return 0 ;
};
