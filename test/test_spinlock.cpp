//----------------------------------------------------------------------------
/// @file test_spinlock.cpp
/// @brief Test program of the class spinlock
///
/// @author Copyright (c) 2010 2012 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <boost/sort/parallel/detail/util/spinlock.hpp>
#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>

#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>

int N =0 ;
void start ( void);
int function1() ;
int function2() ;

using boost::sort::parallel::detail::util::spinlock_t ;
spinlock_t s ;
std::chrono::seconds sec(1);


int test_main( int, char*[] )
{   //----------------- begin ------------
    start ();
    return 0;
};

void start ( void)
{   //---------------------- begin ---------------------
    BOOST_CHECK ( N ==0 );
    //std::cout<<"1.-Begin main function \n";
    N++ ;
    s.try_lock() ;
    N++ ;
    std::thread T1 ( function1);
    std::thread T2 ( function2);
    //std::cout<<"2.-Inside main function \n";
    std::this_thread::sleep_for( std::chrono::seconds(3) ) ;
    BOOST_CHECK ( N == 4 );
    N++ ;
    s.unlock() ;
    T1.join() ;
    T2.join() ;
    BOOST_CHECK ( N == 9);
    //std::cout<<"7.-End of main function \n";
}
int function1()
{   //---------------------------- begin -----------------
    //std::cout<<"2.-Begin function1 \n";
    N++ ;
    std::unique_lock<spinlock_t > ul(s);
    BOOST_CHECK ( N == 5 or N == 7 );
    N++;
    //std::cout<<"4.-Inside function1 \n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    //std::cout<<"6.-End of function1 \n";
    N++ ;
    return 0 ;
}
int function2()
{   //---------------------------- begin -----------------
    //std::cout<<"2.-Begin function2 \n";
    N++ ;
    std::unique_lock<spinlock_t > ul(s);
    BOOST_CHECK ( N == 5 or N == 7 );
    N++ ;
    //std::cout<<"4.-Inside function2 \n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    //std::cout<<"5.-End of function2 \n";
    N++;
    return 0 ;
}

