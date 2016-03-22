//----------------------------------------------------------------------------
/// @file nthread.hpp
/// @brief Description od the class NThreads, which is the variable for to
///        pass to the para llel algorithms the number of threads to use
/// @author Copyright(c) 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_TOOLS_NTHREAD_HPP
#define __BOOST_SORT_PARALLEL_TOOLS_NTHREAD_HPP

#include <thread>
#include <atomic>
#include <cassert>

namespace boost		{
namespace sort		{
namespace parallel	{	
namespace tools		{

//---------------------------------------------------------------------------
/// @class NThread
/// @brief This class encapsulate a number of threads. This number never can
///        be 0.
///        We use this class instead un unsigned for to simplify the calling
///        of concurrent functions where the number of threads is a parameter
/// @remarks The empty construct load the object with the number of HW threads
///          in the actual machine
//---------------------------------------------------------------------------
static const uint32_t NThread_HW = std::thread::hardware_concurrency();
class NThread
{   uint32_t num_th ;

public:
    NThread ( void): num_th ( NThread_HW){};
    NThread ( const NThread & Number):num_th ( Number.num_th){};
    NThread ( uint32_t Nmb ):num_th ((Nmb< 2)?1:Nmb ){};

    uint32_t operator () ( void)const { return num_th;};

    NThread & operator = (const NThread & Number)
    {   num_th = Number.num_th ;
        return *this ;
    };
    NThread & operator= ( uint32_t Nmb)
    {   num_th = ( Nmb < 2)?1:Nmb;
        return *this ;
    };
};

//****************************************************************************
}; // End namespace tools
}; // End namespace parallel
}; // End namespace sort
}; // End namespace boost
//****************************************************************************
#endif
