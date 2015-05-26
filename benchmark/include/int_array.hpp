//----------------------------------------------------------------------------
/// @file int_array.hpp
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
#ifndef __INT_ARRAY_HPP
#define __INT_ARRAY_HPP

#include <cstdint>
#include <iostream>

template <uint32_t NN>
struct int_array
{   uint64_t M[NN];
    //int_array ( const int_array &R )
    //{   for ( uint32_t i =0 ; i < NN ; ++i)
    //        M[i] = R.M[i];
    //};
    int_array ( uint64_t K =0 )
    {   for ( uint32_t i =0 ; i < NN ; ++i)
            M[i] = K;
    };
    //int_array & operator = ( const int_array &R)
    //{   for ( uint32_t i =0 ; i < NN ; ++i)
    //        M[i] = R.M[i];
    //    return *this ;
    //};
    template <class generator >
    static int_array<NN> & generate (int_array<NN> & K , generator & gen)
    {   for ( uint32_t i =0 ; i < NN ; ++i)
            K.M[i] = gen();
        return K ;
    };
    int_array & operator = ( uint64_t K)
    {   for ( uint32_t i =0 ; i < NN ; ++i)
            M[i] = K;
        return *this ;
    };
    uint64_t counter ( void) const
    {   uint64_t Acc =0 ;
        for ( uint32_t i =0 ; i < NN ; Acc += M[i++]) ;
        return Acc ;
    }
    //bool operator < ( const int_array &R) const
    //{   return (M[0]<R.M[0]);
    //};
    bool operator < ( const int_array &R) const
    {   return ( counter() < R.counter());
    };
    ~int_array ()
    {   for ( uint32_t i =0 ; i < NN ; ++i)
            M[i] = 0;
    };
};
template <uint32_t NN>
std::ostream & operator << ( std::ostream & out , const int_array<NN> & N)
{   out<<N.M[0];
    //for ( uint32_t i =0 ; i < NN ; ++i)
    //        out<<N.M[i]<<", ";
    //out<<"\n";
    return out ;
}
#endif // end of int_array.hpp
