//----------------------------------------------------------------------------
/// @file example_03.cpp
/// @brief this program is simple example of the parallel_introsort and
///        sample_sort with the thread  parameter explicitly specified, of
///        the new boost::sort::parallel library
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
using bsp::NThread ;
using bsp::NThread_HW ;


int main( void )
{   //-------------- begin------------
    std::mt19937_64 my_rand(0);

    const uint32_t NMAX = 1000000 ;
    std::vector <uint64_t> A , B ;
    for ( uint32_t i =0 ; i < NMAX ; ++i)
        A.push_back( my_rand() );
    B = A ;
    //------------------------------------------------------------------------
    // If the result of NThread_HW / 6 is smaller than 1, is converted to 1
    //------------------------------------------------------------------------
    bsp::parallel_introsort ( A.begin(), A.end(), NThread ( NThread_HW / 6));
    //------------------------------------------------------------------------
    // NThread ( 100) force to execute with 100 threads
    //------------------------------------------------------------------------
    bsp::sample_sort ( B.begin() , B.end(), NThread ( 100));

    for ( uint32_t i =0 ; i < NMAX ; ++i )
        if ( A[i] != B[i])
            std::cout<<"Error in the sorting process\n";
    return 0 ;
};
