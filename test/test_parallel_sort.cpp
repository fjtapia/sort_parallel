//----------------------------------------------------------------------------
/// @file test_parallel_sort.cpp
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
#include <random>
#include <boost/sort/parallel/algorithm/parallel_sort.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include  <algorithm>

using boost::sort::parallel::util::NThread ;
using boost::sort::parallel::algorithm::parallel_sort ;
using namespace std ;

#define NELEM 500000

void Prueba3() ;

vector<uint32_t> A  ;

int test_main( int, char*[] )
{   //--------------------------- Inicio --------------------------
    std::mt19937_64 my_rand(0);
	A.reserve ( NELEM);

    //------------------------ Inicio -----------------------------
    //cout<<"Sort of "<< NELEM<<" elements in a vector ---------------------\n\n" ;

    //cout<<"Sorted elements -----------------------------------------------\n";
    A.clear() ;
    for ( uint32_t i = 0 ; i < NELEM ; i ++)
        A.push_back( NELEM +i) ;
    Prueba3() ;

    //cout<<"Reverse sorted elements ---------------------------------------\n";
    A.clear() ;
    for ( uint32_t i = 0 ; i < NELEM ; i ++)
        A.push_back( (NELEM<<1) -i) ;
    Prueba3() ;

    //cout<<"Random elements, few repeated ---------------------------------\n";
    A.clear();
    for ( uint32_t i = 0 ; i < NELEM  ; i ++)
        A.push_back(my_rand()) ;
    Prueba3() ;

    //cout<<"Random elements, quite repeated -------------------------------\n";
    A.clear() ;
    for ( uint32_t i = 0 ; i < NELEM  ; i ++)
        A.push_back(my_rand() % (NELEM/2));
    Prueba3() ;

    //cout<<"Random element many repeated ----------------------------------\n";
    A.clear() ;
    for ( uint32_t i = 0 ; i < NELEM  ; i ++)
        A.push_back(my_rand() %10000);
    Prueba3() ;

    //cout<<"Equal elements --------------------------------------------------\n";
    A.clear() ;
    for ( uint32_t i = 0 ; i < NELEM  ; i ++) A.push_back( NELEM);
    Prueba3() ;
    return 0 ;
};

void Prueba3 ( void)
{   //---------------------------- Inicio --------------------------------
	std::vector<uint32_t>  B ;
	B.reserve ( NELEM);

	//cout<<"--------------------- parallel_sort --------------------------\n";
	B = A ;
	parallel_sort (B.begin() , B.end() ,NThread(2) );
	for ( unsigned i = 0 ; i < (B.size() -1) ; i ++ )
	{	BOOST_CHECK ( B[i] <= B[i+1] )  ;
	};
    std::sort ( A.begin(), A.end());
	for ( unsigned i = 0 ; i < A.size()  ; i ++ )
	{	BOOST_CHECK ( B[i] == A[i] ) ;
	};
    //cout<<endl;

};
