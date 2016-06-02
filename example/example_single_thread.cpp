//----------------------------------------------------------------------------
/// @file example_single_thread.cpp
/// @brief this program is simple example of the stable and not stable sort
///        algorithms of the new boost::sort::parallel library
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
#include <vector>

#include <boost/sort/parallel/sort.hpp>

namespace bsp = boost::sort::parallel;

int main( void )
{   //-------------- begin------------
    std::mt19937_64 my_rand(0);

    const uint32_t NMAX = 1000000 ;
    std::vector <uint64_t> A , B ;

    for ( uint32_t i =0 ; i < NMAX ; ++i)   A.push_back( my_rand() );

    B = A ;
    bsp::sort ( A.begin(), A.end());
    bsp::stable_sort ( B.begin() , B.end());

    for ( uint32_t i =0 ; i < NMAX ; ++i )
    {	if ( A[i] != B[i]) std::cout<<"Error in the sorting process\n";
    };
    return 0 ;
};
