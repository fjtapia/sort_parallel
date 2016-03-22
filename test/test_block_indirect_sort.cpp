//----------------------------------------------------------------------------
/// @file test_block_indirect_sort.cpp
/// @brief Test program of the heap sort algorithm
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
#include "boost/sort/parallel/algorithm/block_indirect_sort.hpp"
#include <vector>
#include <random>
#include <algorithm>

namespace bs_util = boost::sort::parallel::util ;
using boost::sort::parallel::algorithm::pbs::block_indirect_sort ;
using boost::sort::parallel::tools::NThread;
using boost::sort::parallel::util::range ;

typedef  typename std::vector<uint64_t>::iterator iter_t ;
typedef  range< iter_t > range_it ;
typedef std::less<uint64_t> compare;


void prueba1 ( void )
{   //-------------- begin------------
	std::vector<uint32_t > VN ;
    std::mt19937 my_rand(0);

    for ( uint32_t i =0 ; i< 130 ; ++i) VN.push_back ( my_rand()%1000);
    range<uint32_t*> R (&VN[0], &VN[130] );
    std::less<uint32_t> comp ;
    block_indirect_sort <4,4, uint32_t *, std::less<uint32_t> > BIS(R, comp,NThread(2));

};


void prueba2 ()
{   //---------------------------- Inicio --------------------------------
    uint64_t  V1[300] ;
    std::less<uint64_t> comp ;

    for ( uint32_t i =0 ; i < 200 ; ++i) V1[i]= i ;
    //std::cout<<"Smart Merge Sort Sorted, not repeated ---------------\n";
    range<uint64_t*> R1 (&V1[0] , &V1[200] );

    block_indirect_sort <4,4, uint64_t *, std::less<uint64_t> > BIS(R1, comp,NThread(2));
    for ( unsigned i = 1 ; i < 200 ; i ++ )
    {	assert ( V1[i-1] <= V1[i] ) ;
    };

    for ( uint32_t i =0 ; i < 200 ; ++i) V1[i] = 199-i ;
    //std::cout<<"Smart Merge Sort Reverse sorted , not repeated ---------------\n";
    R1 = range<uint64_t*>(&V1[0] , &V1[200] );
    block_indirect_sort<4,4, uint64_t *, std::less<uint64_t> > BI2(R1, comp,NThread(2));
    for ( unsigned i = 1 ; i < 200 ; i ++ )
    {	assert ( V1[i-1] <= V1[i] ) ;
    };

    for ( uint32_t i =0 ; i < 300 ; ++i) V1[i] = 299-i ;
    //std::cout<<"Smart Merge Sort Reverse sorted , not repeated ---------------\n";
    R1 = range<uint64_t*>(&V1[0] , &V1[300] );
    block_indirect_sort <4,4, uint64_t *, std::less<uint64_t> > BI3(R1, comp,NThread(2));
    for ( unsigned i = 1 ; i < 300 ; i ++ )
    {	assert ( V1[i-1] <= V1[i] )  ;
    };

    for ( uint32_t i =0 ; i < 300 ; ++i) V1[i] = 88 ;
    //std::cout<<"Smart Merge Sort Equals ---------------\n";
    R1 = range<uint64_t*>(&V1[0] , &V1[300] );
    block_indirect_sort<4,4, uint64_t *, std::less<uint64_t> > BI4(R1, comp,NThread(2));
    for ( unsigned i = 1 ; i < 300 ; i ++ )
    {	assert ( V1[i-1] <= V1[i] ) ;
    };
    return ;
};



void prueba3(void)
{   //----------------------------- begin-------------------------------------

    const uint32_t NElem = 500000 ;
    std::vector<uint64_t>  V1 ;
    std::mt19937_64 my_rand(0);
    compare comp ;

    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back(my_rand() %NElem) ;
    //std::cout<<"smart_merge_sort unsorted ---------------\n";
    range_it R1(  V1.begin() , V1.end() ) ;
    block_indirect_sort <1024,16, iter_t, compare>  BI1(R1, comp,NThread(2));
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	assert ( V1[i-1] <= V1[i] )  ;
    };

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back (i );
    //std::cout<<"smart_merge_sort sorted ---------------\n";
    R1 = range_it (  V1.begin() , V1.end() ) ;
    block_indirect_sort <1024,16, iter_t, compare>  BI2(R1, comp,NThread(2));
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	assert ( V1[i-1] <= V1[i] ) ;
    };

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back( NElem-i) ;
    //std::cout<<"smart_merge_sort reverse sorted ---------------\n";
    R1 = range_it (  V1.begin() , V1.end() ) ;
    block_indirect_sort <1024,16, iter_t, compare>  BI3(R1, comp,NThread(2));
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	assert ( V1[i-1] <= V1[i] ) ;
    };

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back( 1000) ;
    //std::cout<<"smart_merge_sort equals ---------------\n";
    R1 = range_it (  V1.begin() , V1.end() ) ;
    block_indirect_sort <1024,16, iter_t, compare>  BI4(R1, comp,NThread(2));
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	assert ( V1[i-1] == V1[i] ) ;
    };
};


void prueba4 ( void)
{   //------------------------------------ begin -----------------------------
    std::vector <uint64_t > V ;

    for ( uint32_t i =0 ; i < 2083333 ; ++i)  V.push_back ( i);
    range_it R1(  V.begin() , V.end() ) ;
    compare comp ;
    block_indirect_sort  <1024, 64, iter_t,compare > BIS(R1, comp,NThread(8));
    for ( uint32_t i =0 ; i < V.size() ; ++i)
    {   assert ( V[i] == i)  ;
    };
};

void prueba5 ( void)
{   //---------------------- begin ------------------------------------
    const uint32_t NELEM = 416667 ;
    std::vector <uint64_t> A ;
    std::less<uint64_t> comp ;

    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 0);
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 0);

    range_it  R1 ( A.begin() + 1000, A.begin() + (1000+NELEM));
    block_indirect_sort  <1024,16, iter_t,compare > BIS(R1, comp,NThread(4));

    for ( iter_t it =A.begin() + 1000 ; it != A.begin() + (1000+NELEM) ; ++it)
    {   assert ( (*(it-1)) <= (*it)) ;
    };
    assert (A[998] == 0 and A[999] == 0 and A[1000+NELEM] == 0 and A[1001+NELEM] == 0);

    //------------------------------------------------------------------------
    A.clear() ;
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 999999999);
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 999999999);

    R1 = range_it( A.begin() + 1000, A.begin() + (1000+NELEM));
    block_indirect_sort  <1024,16, iter_t,compare > BI2(R1, comp,NThread(4));


    for ( iter_t it =A.begin() + 1001 ; it != A.begin() + (1000+NELEM) ; ++it)
    {   assert ( (*(it-1)) <= (*it));
    };
    assert (A[998] == 999999999 and A[999] == 999999999 and
        A[1000+NELEM] == 999999999 and A[1001+NELEM] == 999999999);
};

void prueba6(void)
{   //----------------------------- begin-------------------------------------
	typedef typename std::vector<uint32_t>::iterator it32_t ;
    const uint32_t NElem = 5000000 ;
    std::vector<uint32_t>  V1 , V2 , V3;
    std::mt19937 my_rand(0);

    typedef std::less<uint32_t> compare ;
    compare  comp ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back(my_rand() ) ;
    V3 = V2 = V1;
    std::sort ( V2.begin() , V2.end() );

    V1 = V3 ;
    //std::cout<<"smart_merge_sort unsorted ---------------\n";
    range<it32_t> R1(  V1.begin() , V1.end() ) ;
    block_indirect_sort<1024,16, it32_t, compare>  BI1(R1, comp,NThread(2));
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	assert ( V1[i] == V2[i] )  ;
    };

    V1 = V3 ;
    //std::cout<<"smart_merge_sort unsorted ---------------\n";
    R1 = range<it32_t>(  V1.begin() , V1.end() ) ;
    block_indirect_sort<1024,16, it32_t, compare>  BI2(R1, comp,NThread(4));
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	assert ( V1[i] == V2[i] )  ;
    };


    V1 = V3 ;
    //std::cout<<"smart_merge_sort unsorted ---------------\n";
    R1 = range<it32_t>(  V1.begin() , V1.end() ) ;
    block_indirect_sort <1024,16, it32_t, compare>  BI3(R1, comp,NThread(8));
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	assert ( V1[i] == V2[i] )  ;
    };

    V1 = V3 ;
    //std::cout<<"smart_merge_sort unsorted ---------------\n";
    R1 = range<it32_t>(  V1.begin() , V1.end() ) ;
    block_indirect_sort <1024,16, it32_t, compare>  BI4(R1, comp,NThread(16));
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	assert ( V1[i] == V2[i] )  ;
    };

    V1 = V3 ;
    //std::cout<<"smart_merge_sort unsorted ---------------\n";
    R1 = range<it32_t>(  V1.begin() , V1.end() ) ;
    block_indirect_sort <1024,16, it32_t, compare>  BI5(R1, comp,NThread(100));
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	assert ( V1[i] == V2[i] )  ;
    };

};
void prueba7( )
{   //--------------------------- Inicio --------------------------
	typedef typename std::vector<uint64_t>::iterator iter_t ;
 	typedef std::less<uint64_t>                      compare;
    const uint32_t NELEM = 10000000 ;
    std::mt19937_64 my_rand(0);
	std::vector<uint64_t> A, B  ;
    A.reserve ( NELEM);
    compare comp ;

    //cout<<"Random element many repeated ----------------------------------\n";
    for ( uint32_t i = 0 ; i < NELEM  ; i ++)
        A.push_back(my_rand() );
    B = A ;
	range<iter_t> RA (A.begin() , A.end()) ;

	block_indirect_sort <1024,64, iter_t, compare> (RA,comp, NThread());
	//parallel_sort_in_place_tag<iter_t,compare> JK (RA , comp,NThread() );
	for ( unsigned i = 0 ; i < (A.size() -1) ; i ++ )
	{	assert ( A[i] <= A[i+1] )  ;
	};
	std::sort ( B.begin() , B.end() );
	for ( unsigned i = 0 ; i < NELEM ; i ++ )
	{	assert ( A[i] == B[i] )  ;
	};

};
int test_main( int, char*[] )
{   //-------------- begin------------
	prueba1() ;
    prueba2() ;
    prueba3() ;
    prueba4() ;
    prueba5() ;
    prueba6() ;
    prueba7 () ;
    return 0 ;
};



