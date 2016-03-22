//----------------------------------------------------------------------------
/// @file test_sort.cpp
/// @brief Benchmark of several sort methods with different uint64_t numbers
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
#include <vector>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/sort/parallel/sort.hpp>

#define NELEM 10000000
using namespace std ;
namespace bp_sort = boost::sort::parallel ;

int test_main (int argc, char *argv[] )
{   //------------------------------ Inicio ----------------------------------
    std::mt19937 my_rand(0);
    vector<uint64_t> A, B, C ;
    B.reserve ( NELEM);

    for ( uint32_t i =0 ; i< NELEM ; ++i) B.push_back ( my_rand());
    C = B ;
    std::sort( C.begin() , C.end());
    //------------------------ Inicio -----------------------------

    A = B ;
    bp_sort::sort (A.begin() , A.end()  );
    BOOST_CHECK ( C.size() == A.size());
    for ( uint32_t i =0 ; i < B.size() ; ++i)
    	BOOST_CHECK ( A[i] == C[i]);

    A = B ;
    bp_sort::stable_sort (A.begin() , A.end()  );
    BOOST_CHECK ( C.size() == A.size());
    for ( uint32_t i =0 ; i < B.size() ; ++i)
    	BOOST_CHECK ( A[i] == C[i]);

    A = B ;
    bp_sort::parallel_sort (A.begin() , A.end()  );
    BOOST_CHECK ( C.size() == A.size());
    for ( uint32_t i =0 ; i < B.size() ; ++i)
    	BOOST_CHECK ( A[i] == C[i]);

    A = B ;
    bp_sort::parallel_stable_sort (A.begin() , A.end()  );
    BOOST_CHECK ( C.size() == A.size());
    for ( uint32_t i =0 ; i < B.size() ; ++i)
    	BOOST_CHECK ( A[i] == C[i]);

    A = B ;
    bp_sort::sample_sort (A.begin() , A.end()  );
    BOOST_CHECK ( C.size() == A.size());
    for ( uint32_t i =0 ; i < B.size() ; ++i)
    	BOOST_CHECK ( A[i] == C[i]);

    return 0 ;
};
