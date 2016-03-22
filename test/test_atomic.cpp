//----------------------------------------------------------------------------
/// @file test_atomic.cpp
/// @brief Test program of the functions for to manipulate atomic variables
///
/// @author Copyright (c) 2010 2012 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/sort/parallel/tools/atomic.hpp>
#include <iostream>

namespace bsp_tools = boost::sort::parallel::tools ;

int test_main( int, char*[] )
{   //----------------- begin ------------
	std::atomic<uint32_t> A ( 7) ;

	std::atomic_uint B  ( 6 ) ;
	auto X = std::atomic_load_explicit<uint32_t> (&A, std::memory_order_acquire );
	//auto X = std::atomic_load (&A );
    std::cout<<X<<std::endl ;
    X = bsp_tools::atomic_read <uint32_t> ( A);
    std::cout<<X<<std::endl ;
    return 0;
};


