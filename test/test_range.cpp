//----------------------------------------------------------------------------
/// @file test_range.cpp
/// @brief
///
/// @author Copyright (c) 2010 2015 Francisco Jos� Tapia (fjtapia@gmail.com )\n
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
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/sort/parallel/util/range.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

using boost::sort::parallel::util::range ;
namespace bs_util = boost::sort::parallel::util ;

template <class iter_t >
std::ostream & operator << ( std::ostream & out , range<iter_t>  R)
{	out<<"[ "<<(R.last- R.first)<<"] ";
	if ( not R.valid() ) return out ;
	while ( R.first != R.last)
		out << (*(R.first++)) <<" ";
	out<<std::endl ;
	return out ;
}

struct xk
{   unsigned tail : 3 ;
    unsigned num  :24 ;
    xk ( unsigned N=0, unsigned T=0):tail(T),num (N){};
    bool operator < ( xk A) const
    {   return ( num < A.num);
    };
};
std::ostream & operator << ( std::ostream & out , xk x)
{	out<<"["<< x.num <<"-"<<x.tail <<"] ";
	return out ;
};
//
//****************************************************************************
//          TEST MOVE, UNINITIALIZED_MOVE, DESTROY
//****************************************************************************
void prueba1 ( void )
{   //-------------------------------------- begin --------------------------
    typedef typename std::vector<uint64_t>::iterator iter_t ;
    typedef range<iter_t>  range_t ;

    std::vector<uint64_t> A , B;

    A.resize ( 10, 0);
    B.resize ( 10, 0);
    for ( uint32_t i =0 ; i < 10 ; ++i) A[i] = i ;
    range_t RA (A.begin() , A.end()), RB ( B.begin() , B.end());
    std::cout<<RA<<std::endl;
	//------------------------------------------------------------------------
	// test copy copy constructor
	//------------------------------------------------------------------------
	range_t RC ( RA);
	BOOST_CHECK ( RC.size() == RA.size());
    std::cout<<RC<<std::endl;
    RC.first = RC.last ;
    RC = RA ;
    std::cout<<RC<<std::endl;
	//-----------------------------------------------------------------------
    //              test of move
    //------------------------------------------------------------------------

    RB = bs_util::init_move( RB, RA);
    for ( uint32_t i =0 ; i < 10 ; ++i)
        BOOST_CHECK ( B[i] == i) ;
    std::cout<<RB<<std::endl;
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
    range <forensic *> F1 (PAux , PAux + 20 );
    std::vector <forensic> V ;
    for ( uint32_t i =0 ; i < 10 ; ++i) V.emplace_back ( i);

    F1 = bs_util::uninit_move ( F1, frange_t(V.begin(),V.end()));
    for ( uint32_t i =0 ; i < 10 ; ++i)
        BOOST_CHECK ( PAux[i].N == i) ;

    bs_util::destroy <forensic *>( F1);
    //for ( uint32_t i =0 ; i < 10 ; ++i)
    //{   if ( PAux[i].N != -1)
    //        std::cout<<"Error in the destroy function \n";
    //};
};
void prueba2 ( )
{	//---------------------------------- begin -----------------------------
	typedef typename std::vector <uint64_t >::iterator iter_t ;
	std::vector <uint64_t > V1 ;
	V1.resize (100, 0);
	range <iter_t> R1 ( V1.begin() , V1.end());
	uint64_t K = 999 ;
	range <iter_t> R2 = init ( R1, K);
	while ( R2.first != R2.last)
		BOOST_CHECK ( * (R2.first ++) == 999);
};
//
//****************************************************************************
//                          TEST OF HALF_MERGE
//****************************************************************************
void prueba3 ()
{   //-------------------------- begin ---------------------------------------
	typedef typename std::vector<uint64_t>::iterator iter_t ;
    typedef range<iter_t> rng ;
    typedef std::less<uint64_t>  compare ;

    compare   comp ;
    std::vector<uint64_t> A , B;
    rng RA (A.begin(), A.end() ), RB ( B.begin() ,B.end());
    rng Rx ( A.begin(), A.end() );
    rng Rz (Rx);

    //------------------------------------------------------------------------
    // src1 empty
    //------------------------------------------------------------------------
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back( i);
    B.clear() ;
    RB = rng ( B.begin() , B.end());
    BOOST_CHECK ( RB.first == RB.last) ;
    RA = rng ( A.begin() , A.end());
    Rx = RA ;
    Rz = half_merge ( Rx, RB,RA, comp);
    BOOST_CHECK ( Rz.size() == 10);
    for ( uint32_t i = 0  ; i < 10 ; ++i)
        BOOST_CHECK ( * (Rz.first + i) == i) ;
    //std::cout<<Rz ;

    //------------------------------------------------------------------------
    // src2 empty
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 10 ; i++) B.push_back( i);
    A.resize (10 , 0);
    Rz = rng ( A.begin() , A.end());
    RB = rng ( B.begin() , B.end());
    RA = rng ( A.end(), A.end());
    Rz = half_merge ( Rx, RB,RA, comp);
    BOOST_CHECK ( Rz.size() == 10);
    for ( uint32_t i = 0  ; i < 10 ; ++i)
        BOOST_CHECK ( * (Rz.first + i) == i) ;
    //std::cout<<Rz ;

    //------------------------------------------------------------------------
    // merged even , odd numbers
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 20 ; i+=2) B.push_back( i);
    BOOST_CHECK ( B.size() == 10 );
    A.resize (10,0);
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.push_back( i);
    BOOST_CHECK ( A.size() == 20 ) ;
    RA = rng(A.begin()+10, A.end() );
    RB = rng( B.begin() ,B.begin()+10);
    Rx = rng ( A.begin(), A.end() );
    Rz = half_merge ( Rx, RB, RA,comp);
    BOOST_CHECK ( Rz.size() == 20 ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( A[i] == i) ;

    //------------------------------------------------------------------------
    // in src1 0-10 in src2 10-20
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0 ; i < 10 ; ++i)B.push_back(i );
    A.resize (10,0);
    for ( uint32_t i =10  ; i < 20 ; i++) A.push_back( i);

    RA = rng(A.begin()+10, A.end() );
    RB = rng( B.begin() ,B.begin()+10);
    Rx = rng ( A.begin(), A.end() );
    Rz = half_merge ( Rx, RB, RA,comp);
    BOOST_CHECK ( Rz.size() == 20 ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( A[i] == i) ;

    //------------------------------------------------------------------------
    // in src2 0-10 in src1 10-20
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =10 ; i < 20 ; ++i)B.push_back(i );
    A.resize (10,0);
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back( i);

    RA = rng(A.begin()+10, A.end() );
    RB = rng( B.begin() ,B.begin()+10);
    Rx = rng ( A.begin(), A.end() );
    Rz = half_merge ( Rx, RB, RA,comp);
    BOOST_CHECK ( Rz.size() == 20 ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)
        BOOST_CHECK ( A[i] == i) ;

};
//
//****************************************************************************
//                          TEST OF STABILITY
//****************************************************************************
void prueba4()
{	//---------------------------------- begin ------------------------------
	typedef typename std::vector<xk>::iterator iter_t ;
    typedef range<iter_t> rng ;
    typedef std::less<xk>  compare ;
    compare   comp ;
    std::vector<xk> A , B;
    rng RA (A.begin(), A.end() ), RB ( B.begin() ,B.end());
    rng Rx ( A.begin(), A.end() );
    rng Rz (Rx);

    //------------------------------------------------------------------------
    // the smallest elements at the beginning of src1
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0 ; i < 50 ; ++i)B.emplace_back(i, 0 );
    for ( uint32_t i =0 ; i < 50 ; ++i)B.emplace_back(100+ i*2, 0 );
    A.resize ( 100, 0 );
    for ( uint32_t i =0  ; i < 100 ; i++) A.emplace_back( 100+ i*2,1);

    RA = rng(A.begin()+100, A.end() );
    RB = rng( B.begin() ,B.begin()+100);
    Rx = rng ( A.begin(), A.end() );
    Rz = half_merge ( Rx, RB, RA,comp);
    BOOST_CHECK ( Rz.size() == 200 ) ;
    std::cout<<Rz<<std::endl ;

    for( uint32_t i =0 ; i < 50 ; ++i)
    	BOOST_CHECK ( A[i].num == i and A[i].tail == 0);
    for ( uint32_t i = 50 ; i < 150 ; ++i)
    {	uint32_t K = i +50 ;
    	uint32_t M = K %2 ;
    	BOOST_CHECK ( A[i].num == K -M  and A[i].tail == M);
    };
    for ( uint32_t i = 150 ; i < 200 ; ++i)
    	BOOST_CHECK ( A[i]. num == 2*i -100 and A[i].tail == 1);

    //------------------------------------------------------------------------
    // the smallest elements at the beginning of src2
    //------------------------------------------------------------------------

    A.clear() ;
    B.clear() ;
    A.resize ( 100, 0 );
    for ( uint32_t i =0 ; i < 50 ; ++i)A.emplace_back(i,1 );
    for ( uint32_t i =0 ; i < 50 ; ++i)A.emplace_back(100+ i*2,1 );

    for ( uint32_t i =0  ; i < 100 ; i++) B.emplace_back( 100+ i*2,0);

    RA = rng(A.begin()+100, A.end() );
    RB = rng( B.begin() ,B.begin()+100);
    Rx = rng ( A.begin(), A.end() );
    Rz = half_merge ( Rx, RB, RA,comp);
    BOOST_CHECK ( Rz.size() == 200 ) ;
    std::cout<<Rz<<std::endl ;

    for( uint32_t i =0 ; i < 50 ; ++i)
    	BOOST_CHECK ( A[i].num == i and A[i].tail == 1);
    for ( uint32_t i = 50 ; i < 150 ; ++i)
    {	uint32_t K = i +50 ;
    	uint32_t M = K %2 ;
    	BOOST_CHECK ( A[i].num == K -M  and A[i].tail == M);
    };
    for ( uint32_t i = 150 ; i < 200 ; ++i)
    	BOOST_CHECK ( A[i]. num == 2*i -100 and A[i].tail == 0);
}
//
//****************************************************************************
//                    TEST OF FULL_MERGE
//****************************************************************************
void prueba5 ()
{   //---------------------------- begin -------------------------------------
    typedef std::less<uint64_t>  compare ;
    typedef range<uint64_t *> rng ;

    std::vector<uint64_t> A , B;
    compare comp ;

    A.clear() ;
    B.clear() ;
    B.assign ( 20 , 0 );
    for ( uint32_t i =0  ; i < 20 ; i+=2) A.push_back( i);
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.push_back( i);

    rng A1 (& A[0], & A[10]), A2 (&A[10], & A[20] );
    rng B1 ( & B[0], & B[20]);
    rng C1 ( A1);

    C1 = full_merge ( B1, A1, A2, comp )  ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)   BOOST_CHECK ( B[i] == i);

    A.clear() ;
    B.clear() ;
    B.assign ( 20 , 100 );
    for ( uint32_t i =0  ; i < 20 ; i++) A.push_back (i);

    full_merge ( B1,rng( &A[0],&A[10]) ,rng(&A[10],&A[20]),comp)  ;
    for ( uint32_t i = 0  ; i < 20 ; ++i) BOOST_CHECK ( B[i] == i);

    A.clear() ;
    B.clear() ;
    B.assign ( 20 , 100 );

    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back (10+i);
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back(i);

    full_merge ( B1,rng( &A[0],&A[10]),rng(&A[10],&A[20]),comp );
    for ( uint32_t i = 0  ; i < 20 ; ++i)    BOOST_CHECK ( B[i] == i) ;

};

//
//****************************************************************************
//                          TEST OF FULL_MERGE
//****************************************************************************
void prueba6 ()
{   //-------------------------- begin ---------------------------------------
	typedef typename std::vector<uint64_t>::iterator iter_t ;
    typedef range<iter_t> rng ;
    typedef std::less<uint64_t>  compare ;

    compare   comp ;
    std::vector<uint64_t> A , B;
    rng RA1 (A.begin(), A.end() ),RA2 (A.begin(), A.end() );
    rng RB ( B.begin() ,B.end());
    rng Rz (RB);

    //------------------------------------------------------------------------
    // src1 empty
    //------------------------------------------------------------------------
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back( i);
    B.clear() ;
    B.resize ( 20 , 100);

    RB = rng ( B.begin() , B.end());
    RA1 = rng ( A.begin() , A.begin());
    RA2 = rng ( A.begin() , A.end());

    Rz = full_merge ( RB, RA1,RA2, comp);
    BOOST_CHECK ( Rz.size() == 10);
    for ( uint32_t i = 0  ; i < 10 ; ++i)
        BOOST_CHECK ( * (Rz.first + i) == i) ;


    //------------------------------------------------------------------------
    // src2 empty
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back( i);
    B.resize (10 , 0);

    RA1 = rng ( A.begin() , A.end());
    RA2 = rng ( A.end(), A.end() );
    RB = rng ( B.begin() , B.end());

    Rz = full_merge ( RB, RA1,RA2, comp);
    BOOST_CHECK ( Rz.size() == 10);
    for ( uint32_t i = 0  ; i < 10 ; ++i)
        BOOST_CHECK ( * (Rz.first + i) == i) ;

    //------------------------------------------------------------------------
    // merged even , odd numbers
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 20 ; i+=2) A.push_back( i);
    BOOST_CHECK ( A.size() == 10 );
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.push_back( i);
    BOOST_CHECK ( A.size() == 20 );
    B.resize (20,0);

    RA1 = rng( A.begin() ,A.begin()+10);
    RA2 = rng(A.begin()+10, A.end() );
    RB  = rng ( B.begin() , B.end());

    Rz = full_merge ( RB, RA1, RA2,comp);
    BOOST_CHECK ( Rz.size() == 20 ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)  BOOST_CHECK ( B[i] == i) ;

    //------------------------------------------------------------------------
    // in src1 0-10 in src2 10-20
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0 ; i < 20 ; ++i)A.push_back(i );
    B.resize (20,0);

    RA1 = rng( A.begin() ,A.begin()+10);
    RA2 = rng(A.begin()+10, A.end() );
    RB  = rng ( B.begin() , B.end());

    Rz = full_merge ( RB, RA1, RA2,comp);
    BOOST_CHECK ( Rz.size() == 20 ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)   BOOST_CHECK ( B[i] == i) ;

    //------------------------------------------------------------------------
    // in src2 0-10 in src1 10-20
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =10 ; i < 20 ; ++i)A.push_back(i );
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back( i);
    B.resize (20,0);

    RA1 = rng( A.begin() ,A.begin()+10);
    RA2 = rng(A.begin()+10, A.end() );
    RB  = rng ( B.begin() , B.end());

    Rz = full_merge ( RB, RA1, RA2,comp);
    BOOST_CHECK ( Rz.size() == 20 ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)    BOOST_CHECK ( B[i] == i) ;

};

//
//****************************************************************************
//                          TEST OF STABILITY
//****************************************************************************

void prueba7()
{	//---------------------------------- begin ------------------------------
	typedef typename std::vector<xk>::iterator iter_t ;
    typedef range<iter_t> rng ;
    typedef std::less<xk>  compare ;
    compare   comp ;
    std::vector<xk> A , B;
    rng RA1 (A.begin(), A.end() ), RA2 (A.begin(), A.end() );
    rng RB ( B.begin() ,B.end());
    rng Rz (RB);

    //------------------------------------------------------------------------
    // the smallest elements at the beginning of src1
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0 ; i < 50 ; ++i)A.emplace_back(i, 0 );
    for ( uint32_t i =0 ; i < 50 ; ++i)A.emplace_back(100+ i*2, 0 );
    for ( uint32_t i =0  ; i < 100 ; i++) A.emplace_back( 100+ i*2,1);
    B.resize ( 200, 0 );

    RA1 = rng( A.begin() ,A.begin()+100);
    RA2 = rng(A.begin()+100, A.end() );
    RB = rng ( B.begin() , B.end());

    Rz = full_merge ( RB, RA1, RA2,comp);
    BOOST_CHECK ( Rz.size() == 200 ) ;

    for( uint32_t i =0 ; i < 50 ; ++i)
    	BOOST_CHECK ( B[i].num == i and B[i].tail == 0);
    for ( uint32_t i = 50 ; i < 150 ; ++i)
    {	uint32_t K = i +50 ;
    	uint32_t M = K %2 ;
    	BOOST_CHECK ( B[i].num == K -M  and B[i].tail == M);
    };
    for ( uint32_t i = 150 ; i < 200 ; ++i)
    	BOOST_CHECK ( B[i]. num == 2*i -100 and B[i].tail == 1);

    //------------------------------------------------------------------------
    // the smallest elements at the beginning of src2
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 100 ; i++) A.emplace_back( 100+ i*2,0);
    for ( uint32_t i =0 ; i < 50 ; ++i)A.emplace_back(i,1 );
    for ( uint32_t i =0 ; i < 50 ; ++i)A.emplace_back(100+ i*2,1 );
    B.resize ( 200, 0 );

    RA1 = rng( A.begin() ,A.begin()+100);
    RA2 = rng(A.begin()+100, A.end() );
    RB = rng ( B.begin() , B.end());

    Rz = full_merge ( RB, RA1, RA2,comp);
    BOOST_CHECK ( Rz.size() == 200 ) ;

    for( uint32_t i =0 ; i < 50 ; ++i)
    	BOOST_CHECK ( B[i].num == i and B[i].tail == 1);
    for ( uint32_t i = 50 ; i < 150 ; ++i)
    {	uint32_t K = i +50 ;
    	uint32_t M = K %2 ;
    	BOOST_CHECK ( B[i].num == K -M  and B[i].tail == M);
    };
    for ( uint32_t i = 150 ; i < 200 ; ++i)
    	BOOST_CHECK ( B[i]. num == 2*i -100 and B[i].tail == 0);
}

//
//****************************************************************************
//      TEST OF UNINITIALIZED_FULL_MERGE
//****************************************************************************
void prueba8 ()
{   //---------------------------- begin -------------------------------------
    typedef std::less<uint64_t>  compare ;
    typedef range<uint64_t *> rng ;

    std::vector<uint64_t> A , B;
    compare comp ;

    A.clear() ;
    B.clear() ;
    B.assign ( 20 , 0 );
    for ( uint32_t i =0  ; i < 20 ; i+=2) A.push_back( i);
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.push_back( i);

    rng A1 (& A[0], & A[10]), A2 (&A[10], & A[20] );
    rng B1 ( & B[0], & B[20]);
    rng C1 ( A1);

    C1 = uninit_full_merge ( B1, A1, A2, comp )  ;
    for ( uint32_t i = 0  ; i < 20 ; ++i) BOOST_CHECK ( B[i] == i);

    A.clear() ;
    B.clear() ;
    B.assign ( 20 , 100 );
    for ( uint32_t i =0  ; i < 20 ; i++) A.push_back (i);

    uninit_full_merge ( B1,rng( &A[0],&A[10]) ,rng(&A[10],&A[20]),comp)  ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)  BOOST_CHECK ( B[i] == i);

    A.clear() ;
    B.clear() ;
    B.assign ( 20 , 100 );

    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back (10+i);
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back(i);
    uninit_full_merge ( B1,rng( &A[0],&A[10]),rng(&A[10],&A[20]),comp );
    for ( uint32_t i = 0  ; i < 20 ; ++i) BOOST_CHECK ( B[i] == i) ;
};

//
//****************************************************************************
//                          TEST OF FULL_MERGE
//****************************************************************************
void prueba9 ()
{   //-------------------------- begin ---------------------------------------
	typedef typename std::vector<uint64_t>::iterator iter_t ;
    typedef range<iter_t> rng ;
    typedef std::less<uint64_t>  compare ;

    compare   comp ;
    std::vector<uint64_t> A , B;
    rng RA1 (A.begin(), A.end() ),RA2 (A.begin(), A.end() );
    range<uint64_t*> RB ( &B[0] ,&B[0]);
    range<uint64_t*> Rz (RB);

    //------------------------------------------------------------------------
    // src1 empty
    //------------------------------------------------------------------------
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back( i);
    B.clear() ;
    B.resize ( 20 , 100);

    RB = range<uint64_t*> ( &B[0] ,&B[20]);
    RA1 = rng ( A.begin() , A.begin());
    RA2 = rng ( A.begin() , A.end());

    Rz = uninit_full_merge ( RB, RA1,RA2, comp);
    BOOST_CHECK ( Rz.size() == 10);
    for ( uint32_t i = 0  ; i < 10 ; ++i)
        BOOST_CHECK ( * (Rz.first + i) == i) ;

    //------------------------------------------------------------------------
    // src2 empty
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back( i);
    B.resize (10 , 0);

    RA1 = rng ( A.begin() , A.end());
    RA2 = rng ( A.end(), A.end() );
    RB = range<uint64_t*> ( &B[0] ,&B[10]);

    Rz = uninit_full_merge ( RB, RA1,RA2, comp);
    BOOST_CHECK ( Rz.size() == 10);
    for ( uint32_t i = 0  ; i < 10 ; ++i)
        BOOST_CHECK ( * (Rz.first + i) == i) ;

    //------------------------------------------------------------------------
    // merged even , odd numbers
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 20 ; i+=2) A.push_back( i);
    BOOST_CHECK ( A.size() == 10 );
    for ( uint32_t i =1  ; i < 20 ; i+=2) A.push_back( i);
    BOOST_CHECK ( A.size() == 20 );
    B.resize (20,0);

    RA1 = rng( A.begin() ,A.begin()+10);
    RA2 = rng(A.begin()+10, A.end() );
    RB  = range<uint64_t*> ( &B[0] ,&B[20]);

    Rz = uninit_full_merge ( RB, RA1, RA2,comp);
    BOOST_CHECK ( Rz.size() == 20 ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)  BOOST_CHECK ( B[i] == i) ;

    //------------------------------------------------------------------------
    // in src1 0-10 in src2 10-20
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0 ; i < 20 ; ++i)A.push_back(i );
    B.resize (20,0);

    RA1 = rng( A.begin() ,A.begin()+10);
    RA2 = rng(A.begin()+10, A.end() );
    RB  = range<uint64_t*> ( &B[0] ,&B[20]);

    Rz = uninit_full_merge ( RB, RA1, RA2,comp);
    BOOST_CHECK ( Rz.size() == 20 ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)  BOOST_CHECK ( B[i] == i) ;

    //------------------------------------------------------------------------
    // in src2 0-10 in src1 10-20
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =10 ; i < 20 ; ++i)A.push_back(i );
    for ( uint32_t i =0  ; i < 10 ; i++) A.push_back( i);
    B.resize (20,0);

    RA1 = rng( A.begin() ,A.begin()+10);
    RA2 = rng(A.begin()+10, A.end() );
    RB  = range<uint64_t*> ( &B[0] ,&B[20]);

    Rz = uninit_full_merge ( RB, RA1, RA2,comp);
    BOOST_CHECK ( Rz.size() == 20 ) ;
    for ( uint32_t i = 0  ; i < 20 ; ++i)  BOOST_CHECK ( B[i] == i) ;
};

//
//****************************************************************************
//                          TEST OF STABILITY
//****************************************************************************

void prueba10()
{	//---------------------------------- begin ------------------------------
	typedef typename std::vector<xk>::iterator iter_t ;
    typedef range<iter_t> rng ;
    typedef std::less<xk>  compare ;
    compare   comp ;
    std::vector<xk> A , B;
    rng RA1 (A.begin(), A.end() ), RA2 (A.begin(), A.end() );
    range <xk*> RB ;
    range <xk*> Rz ;

    //------------------------------------------------------------------------
    // the smallest elements at the beginning of src1
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0 ; i < 50 ; ++i)A.emplace_back(i, 0 );
    for ( uint32_t i =0 ; i < 50 ; ++i)A.emplace_back(100+ i*2, 0 );
    for ( uint32_t i =0  ; i < 100 ; i++) A.emplace_back( 100+ i*2,1);
    B.resize ( 200, 0 );

    RA1 = rng( A.begin() ,A.begin()+100);
    RA2 = rng(A.begin()+100, A.end() );
    RB = range <xk*> ( &B[0] , &B[200]);

    Rz = uninit_full_merge ( RB, RA1, RA2,comp);
    BOOST_CHECK ( Rz.size() == 200 ) ;


    for( uint32_t i =0 ; i < 50 ; ++i)
    	BOOST_CHECK ( B[i].num == i and B[i].tail == 0);
    for ( uint32_t i = 50 ; i < 150 ; ++i)
    {	uint32_t K = i +50 ;
    	uint32_t M = K %2 ;
    	BOOST_CHECK ( B[i].num == K -M  and B[i].tail == M);
    };
    for ( uint32_t i = 150 ; i < 200 ; ++i)
    	BOOST_CHECK ( B[i]. num == 2*i -100 and B[i].tail == 1);

    //------------------------------------------------------------------------
    // the smallest elements at the beginning of src2
    //------------------------------------------------------------------------
    A.clear() ;
    B.clear() ;
    for ( uint32_t i =0  ; i < 100 ; i++) A.emplace_back( 100+ i*2,0);
    for ( uint32_t i =0 ; i < 50 ; ++i)A.emplace_back(i,1 );
    for ( uint32_t i =0 ; i < 50 ; ++i)A.emplace_back(100+ i*2,1 );
    B.resize ( 200, 0 );

    RA1 = rng( A.begin() ,A.begin()+100);
    RA2 = rng(A.begin()+100, A.end() );
    RB = range <xk*> ( &B[0] , &B[200]);

    Rz = uninit_full_merge ( RB, RA1, RA2,comp);
    BOOST_CHECK ( Rz.size() == 200 ) ;

    for( uint32_t i =0 ; i < 50 ; ++i)
    	BOOST_CHECK ( B[i].num == i and B[i].tail == 1);
    for ( uint32_t i = 50 ; i < 150 ; ++i)
    {	uint32_t K = i +50 ;
    	uint32_t M = K %2 ;
    	BOOST_CHECK ( B[i].num == K -M  and B[i].tail == M);
    };
    for ( uint32_t i = 150 ; i < 200 ; ++i)
    	BOOST_CHECK ( B[i]. num == 2*i -100 and B[i].tail == 0);
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

    return 0 ;
};
