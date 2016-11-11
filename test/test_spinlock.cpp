//----------------------------------------------------------------------------
/// @file test_spinlock.cpp
/// @brief Test program of the class spinlock
///
/// @author Copyright (c) 2016 Francisco Jose Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanying file LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <ciso646>
#include <chrono>
#include <mutex>
#include <thread>
#include <boost/sort/parallel/detail/util/spinlock.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>

int N = 0;
void start (void);
int function1 ( );
int function2 ( );

using boost::sort::parallel::detail::util::spinlock_t;
spinlock_t s;
std::chrono::seconds sec (1);

int test_main (int, char *[])
{
    start ( );
    return 0;
};

void start (void)
{
    BOOST_CHECK (N == 0);

    N++;
    s.try_lock ( );
    N++;
    std::thread T1 (function1);
    std::thread T2 (function2);

    std::this_thread::sleep_for (std::chrono::seconds (3));
    BOOST_CHECK (N == 4);
    N++;
    s.unlock ( );
    T1.join ( );
    T2.join ( );
    BOOST_CHECK (N == 9);
}
int function1 ( )
{
    N++;
    std::unique_lock< spinlock_t > ul (s);
    BOOST_CHECK (N == 5 or N == 7);

    N++;
    std::this_thread::sleep_for (std::chrono::seconds (2));
    N++;
    return 0;
}
int function2 ( )
{
    N++;
    std::unique_lock< spinlock_t > ul (s);
    BOOST_CHECK (N == 5 or N == 7);
    N++;

    std::this_thread::sleep_for (std::chrono::seconds (1));
    N++;
    return 0;
}
