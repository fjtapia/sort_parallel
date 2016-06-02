//----------------------------------------------------------------------------
/// @file test_intro_sort.cpp
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
#include <iostream>
#include <algorithm>
#include <random>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include "boost/sort/parallel/detail/intro_sort.hpp"

using namespace boost::sort::parallel::detail ;
using namespace std ;


void test01( void)
{	//---------------------------- Inicio --------------------------------
	std::less<uint64_t> comp ;
	std::vector<uint64_t> A ,B;
    A={7,4,23,15,17,2,24,13,8,3,11,16, 6, 14,21,5,1,12,19,22,25,8};
	cout<<"Intro Sort unsorted, not repeated ----------------------------\n";
	intro_sort ( &A[0] , &A[22], comp );
	for ( unsigned i = 0 ; i < 21 ; i ++ )
	{	BOOST_CHECK ( A[i] <= A[i+1] )  ;
	};

    B={1,2,3,5,6,7,8,9,10,11,12,13,14,15,17,18,19,20,21,23,24,25};
	cout<<"Intro Sort sorted not repeated ----------------------------\n";
	intro_sort ( &B[0] , &B[22], comp );
	for ( unsigned i = 0 ; i < 21 ; i ++ )
	{	BOOST_CHECK ( B[i] <= B[i+1] )  ;
	};

    unsigned C [] ={27,26,25,23,22,21,19,18,17,16,15,14,13,11,10,9,8,7,6,5,3,2};
	cout<<"Intro Sort reverse sorted , not repeated------------------------\n";
	intro_sort (&C[0] , &C[22], comp );
	for ( unsigned i = 0 ; i < 21 ; i ++ )
	{	BOOST_CHECK ( C[i] <=C[i+1] )  ;
	};

    unsigned D [] ={4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
                    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4};
	cout<<"Intro Sort Equals ----------------------------\n";
	intro_sort ( &D[0] , &D[44], comp);
	for ( unsigned i = 0 ; i < 43 ; i ++ )
	{	BOOST_CHECK ( D[i] <= D[i+1] )  ;
	};

	cout<<"Intro Sort 100 Random elements ----------------------------\n";
	unsigned F[100];
	for ( unsigned i = 0 ; i < 100 ; i ++) F[i] = rand()%1000 ;
	intro_sort ( &F[0] , &F[100], comp );
	for ( unsigned i = 0 ; i < 99 ; i ++ )
	{	BOOST_CHECK ( F[i] <= F[i+1] )  ;
	};

	const unsigned NG= 10000;
	cout<<"Intro Sort "<<NG<<" Random elements----------------------------\n";
	unsigned G[NG];
	for ( unsigned i = 0 ; i < NG ; i ++) G[i] = rand()%1000 ;
	intro_sort ( &G[0] , &G[NG], comp );
	for ( unsigned i = 0 ; i < NG-1 ; i ++ )
	{	BOOST_CHECK ( G[i] <= G[i+1] ) ;
	};


    A ={7 ,4 ,23,15,17,2 ,24,13,8 ,3 ,11,16,6 ,14,21,5 ,1 ,12,19,22,
        25,8 ,34,36,29,43,52,47,1 ,26,29,41,44,50,47,49,30,55,48,47};
    B = A ;
	//cout<<"intro_sort, unordered, not repeated ---------------\n";
	intro_sort ( A.begin() , A.end(), std::less<uint64_t>() );
	std::sort ( B.begin() , B.end());

	for ( unsigned i = 0 ; i < A.size() ; i ++ )
	{	//std::cout<<B[i]<<"  "<<A[i]<<std::endl ;
		BOOST_CHECK ( B[i] == A[i] )  ;
	};

    A={1,2,3,5,6,7,8,9,10,11,12,13,14,15,17,18,19,20,21,23,
       24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,40, 39 };
    B = A ;
	//cout<<"intro_sort, ordered,  not repeated ------------------\n";
	intro_sort ( A.begin() , A.end() , comp);
	std::sort ( B.begin() , B.end());
	for ( unsigned i = 0 ; i < A.size() ; i ++ )
	{	BOOST_CHECK ( B[i] == A[i] )  ;
	};

    A ={40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,23,
        22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,3,2,1,0   };
    B =  A ;
	//cout<<"intro_sort, reverse sorted, not repeated ---------\n";
	intro_sort ( A.begin() , A.end(), comp );
	std::sort ( B.begin() , B.end());
	for ( unsigned i = 0 ; i < A.size() ; i ++ )
	{	BOOST_CHECK ( B[i] == A[i] )  ;
	};


    A ={4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
       4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,};
    B = A ;
	//cout<<"intro_sort, equal elements -----------------------\n";
	intro_sort ( A.begin() , A.end(), comp );
	std::sort ( B.begin() , B.end());
	for ( unsigned i = 0 ; i < A.size() ; i ++ )
	{	BOOST_CHECK ( B[i] == A[i] ) ;
	};


	//cout<<"intro_sort 100 random elements -----------------\n";
	static const unsigned NF = 1000000 ;
	//cout<<"intro_sort "<<NF<<" random elements -----------------\n";
	A.clear() ;
	for ( unsigned i = 0 ; i < NF ; i ++) A.push_back ( rand()) ;
	B =  A ;
	intro_sort ( A.begin() , A.end(), comp );
	std::sort ( B.begin() , B.end());
	for ( unsigned i = 0 ; i < A.size() ; i ++ )
	{	BOOST_CHECK ( A[i] == B[i] ) ;
	};


	const unsigned NH= 1000;
	//cout<<"intro_sort "<<NH<<" equal elements ----------------\n";
    A.clear() ;
	for ( unsigned i = 0 ; i < NH ; i ++) A.push_back( 7) ;
	intro_sort ( A.begin() , A.end(), comp );
	for ( unsigned i = 1 ; i < A.size() ; i ++ )
	{	BOOST_CHECK ( 7 == A[i] )  ;
	};

};
void test02 ( void)
{   //---------------------- begin ------------------------------------
	std::less<uint64_t> comp ;
    typedef typename std::vector<uint64_t>::iterator iter_t ;
    const uint32_t NELEM = 416667 ;
    std::vector <uint64_t> A ;

    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 0);
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 0);

    intro_sort ( A.begin() + 1000, A.begin() + (1000+NELEM), comp );

    for ( iter_t it =A.begin() + 1000 ; it != A.begin() + (1000+NELEM) ; ++it)
    {   BOOST_CHECK ( (*(it-1)) <= (*it)) ;
    };
    BOOST_CHECK (A[998] == 0 and A[999] == 0 and
    		     A[1000+NELEM] == 0 and A[1001+NELEM] == 0);

    //------------------------------------------------------------------------
    A.clear() ;
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 999999999);
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 999999999);

    intro_sort ( A.begin() + 1000, A.begin() + (1000+NELEM), comp);

    for ( iter_t it =A.begin() + 1001 ; it != A.begin() + (1000+NELEM) ; ++it)
    {   BOOST_CHECK ( (*(it-1)) <= (*it));
    };
    BOOST_CHECK (A[998] == 999999999 and A[999] == 999999999 and
        A[1000+NELEM] == 999999999 and A[1001+NELEM] == 999999999);


};
void test03 (void)
{   //--------------------------- Inicio --------------------------
	std::less<uint64_t> comp ;
	std::vector<uint64_t> A ,B;
    std::mt19937_64 my_rand(0);
    uint32_t NELEM = 3000000;
    A.reserve ( NELEM);
    A.clear();
    for ( uint32_t i = 0 ; i < NELEM  ; i ++)  A.push_back(my_rand()) ;
    B =  A ;
	intro_sort ( A.begin() , A.end() , comp);
	std::sort ( B.begin() , B.end());
	for ( unsigned i = 0 ; i < A.size() ; i ++ )
	{	BOOST_CHECK ( A[i] == B[i] ) ;
	};
};
int test_main( int, char*[] )
{
    test01() ;
    test02() ;
    test03() ;
    return 0 ;
};
