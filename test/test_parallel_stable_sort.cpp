//----------------------------------------------------------------------------
/// @file test_parallel_stable_sort.cpp
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
//#include <iostream>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/sort/parallel/detail/parallel_stable_sort.hpp>
#include <vector>

#include <algorithm>

using namespace std ;
namespace bsort = boost::sort::parallel::detail ;


typedef typename std::vector<uint64_t>::iterator iter_t ;


std::mt19937_64 my_rand(0);

struct xk
{   unsigned tail : 3 ;
    unsigned num  :24 ;
    bool operator < ( xk A) const
    {   return ( num < A.num);
    };
};
//std::ostream & operator <<( std::ostream & salida, const xk &s)
//{   salida<<"["<<s.num<<" : "<<s.tail<<"] ";
//    return salida ;
//};

void test3 ( )
{   //---------------------------------- begin ---------------------------
	typedef typename std::vector< xk>::iterator iter_t;
	typedef std::less<xk>						compare ;

    const uint32_t NMAX = 500000 ;
    std::vector< xk> V1, V2;
    V1.reserve ( NMAX);
    for ( uint32_t i = 0 ; i < 8 ; ++i)
    {   for ( uint32_t k =0 ; k < NMAX; ++k)
        {   uint32_t NM = my_rand() ;
            xk G ;
            G.num = NM >> 3 ;
            G.tail = i ;
            V1.push_back(G);
        };
    };
    V2 = V1 ;
    bsort::parallel_stable_sort<iter_t,compare> ( V1.begin() , V1.end());
    std::stable_sort( V2.begin() , V2.end() );

    //------------------------------------------------------------------------
    //            Comprobation
    //------------------------------------------------------------------------
    BOOST_CHECK ( V1.size() == V2.size()) ;
    for ( uint32_t i = 0 ; i < V1.size() ; ++i)
    {   BOOST_CHECK ( V1[i].num == V2[i].num and V1[i].tail == V2[i].tail);
    };

};

void test4(void)
{   //----------------------------- begin-------------------------------------
	typedef typename std::vector<uint64_t>::iterator iter_t ;
	typedef std::less<uint64_t>                      compare ;

    const uint32_t NElem = 500000 ;
    std::vector<uint64_t>  V1 ;
        std::mt19937_64 my_rand(0);

    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back(my_rand() %NElem) ;
	//cout<<"parallel_stable_sort unsorted ---------------\n";
	bsort::parallel_stable_sort<iter_t, compare> (  V1.begin() , V1.end() );
	for ( unsigned i = 1 ; i < NElem ; i ++ )
	{	BOOST_CHECK ( V1[i-1] <= V1[i] )  ;
	};

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back (i );
	//cout<<"parallel_stable_sort sorted ---------------\n";
	bsort::parallel_stable_sort<iter_t, compare> (  V1.begin() , V1.end() );
	for ( unsigned i = 1 ; i < NElem ; i ++ )
	{	BOOST_CHECK ( V1[i-1] <= V1[i] ) ;
	};

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back( NElem-i) ;
	//cout<<"parallel_stable_sort reverse sorted ---------------\n";
	bsort::parallel_stable_sort<iter_t, compare> (  V1.begin() , V1.end() );
	for ( unsigned i = 1 ; i < NElem ; i ++ )
	{	BOOST_CHECK ( V1[i-1] <= V1[i] ) ;
	};

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back( 1000) ;
	//cout<<"parallel_stable_sort equals ---------------\n";
	bsort::parallel_stable_sort<iter_t, compare> (  V1.begin() , V1.end() );
	for ( unsigned i = 1 ; i < NElem ; i ++ )
	{	BOOST_CHECK ( V1[i-1] == V1[i] )  ;
	};
};

void test5 ( void)
{   //---------------------- begin ------------------------------------
	typedef typename std::vector<uint64_t>::iterator iter_t ;
	typedef std::less<uint64_t>                      compare ;

    const uint32_t NELEM = 500000 ;
    std::vector <uint64_t>  A ,B;
    A.reserve ( NELEM) ;

    for ( unsigned i = 0  ; i < NELEM ; i ++ )
		A.push_back(my_rand() )  ;
    B = A ;
    //cout<<"--------------------- parallel_stable_sort----------------\n";
	bsort::parallel_stable_sort<iter_t, compare> (A.begin() , A.end()  );
	for ( unsigned i = 0 ; i < (NELEM -1) ; i ++ )
	{	BOOST_CHECK ( A[i] <= A[i+1] )  ;
	};
	std::stable_sort ( B.begin() , B.end());
	BOOST_CHECK ( A.size() == B.size());

    for ( uint32_t i = 0 ; i < A.size() ;++i)
        BOOST_CHECK  ( A[i] == B[i]) ;
    //cout<<endl;
};

void test6 ( void)
{   //---------------------- begin ------------------------------------
	typedef typename std::vector<uint64_t>::iterator iter_t ;
	typedef std::less<uint64_t>                      compare ;

    const uint32_t NELEM = 500000 ;
    std::vector <uint64_t>  A ;
    A.reserve ( NELEM) ;

    for ( unsigned i = 0  ; i < NELEM ; i ++ )
		A.push_back(NELEM - i )  ;

    //cout<<"--------------------- parallel_stable_sort----------------\n";
	bsort::parallel_stable_sort<iter_t, compare> (A.begin() , A.end()  );
	for ( unsigned i = 1 ; i < NELEM ; i ++ )
	{	BOOST_CHECK ( A[i-1] <= A[i] ) ;
	};

    //cout<<endl;
};


int test_main( int, char*[] )
{
	test3() ;
	test4() ;
	test5() ;
	test6() ;
	return 0 ;

}
