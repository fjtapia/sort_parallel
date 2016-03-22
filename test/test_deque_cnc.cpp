//----------------------------------------------------------------------------
/// @file test_deque_cnc.cpp
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
#include <iostream>
#include <thread>
#include <mutex>

#include <boost/sort/parallel/tools/deque_cnc.hpp>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>

namespace bsp_tools = boost::sort::parallel::tools ;
using std::cout ;
using std::endl;
using bsp_tools::deque_cnc ;


#define NELEM 5000000

static const uint32_t NC = std::thread::hardware_concurrency() ;
static const uint32_t NCores = (NC >8) ? 8 : NC ;
deque_cnc<int> S;

void function1() ;
void function2() ;
void prueba_cnc ( void);
void prueba ( void);


int test_main( int, char*[] )
{   //----------------- begin ------------
    prueba_cnc ();
    return 0;
};

void function1()
{   //---------------------------- begin -----------------
    for ( int i =0 ; i < NELEM ; ++i)
        S.push_back(i);
};
void function2()
{   //---------------------------- begin -----------------
    int k ;
    while ( S.pop_copy_back(k) );
};


void prueba_cnc ( void)
{   //--------------------------------- begin ------------------
    //S.reserve ( (int)NELEM * (int)NCores  );
    //double duracion ;

    std::thread  T [8] ;

    for (uint32_t i =0 ; i < NCores ; ++i)
    {   T[i] = std::thread ( function1 ) ;
    };
    for (uint32_t i =0 ; i < NCores ; ++i)
    {   T[i].join();
    };
;
    //duracion =  subtract_time(finish , start) ;
    //cout<<"Time spent :"<<duracion<<" seconds\n";
    //cout<<"The size of S is :"<<S.size()<<endl;

    for (uint32_t i =0 ; i < NCores ; ++i)
    {   T[i] = std::thread ( function2 ) ;
    };
    for (uint32_t i =0 ; i < NCores ; ++i)
    {   T[i].join();
    };

    //duracion =  subtract_time(finish , start) ;
    //cout<<"Time spent :"<<duracion<<" seconds\n";
    //cout<<"The size of S is :"<<S.size()<<endl;

};

