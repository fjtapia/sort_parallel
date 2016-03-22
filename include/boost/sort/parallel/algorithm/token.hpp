//----------------------------------------------------------------------------
/// @file token.hpp
/// @brief Insertion Sort algorithm
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_ALGORITHM_TOKEN_HPP
#define __BOOST_SORT_PARALLEL_ALGORITHM_TOKEN_HPP

namespace boost		{
namespace sort		{
namespace parallel	{
namespace algorithm {
//
///---------------------------------------------------------------------------
/// @struct token
/// @brief pair of iterators. This struct is used by several sort algorithms
/// @tparam iter_t : iterator to the elements to sort
/// @remarks
//----------------------------------------------------------------------------
template <class iter_t>
struct token
{   //----------- variables -------------
    iter_t first ;
    iter_t last ;
    //------------------- Functions -------------------
    token( void){} ;
    token ( iter_t it_f , iter_t it_l):first ( it_f), last ( it_l){};
    bool   	valid 		( void ) const { return ((last-first)>= 0 ); };
    size_t 	size		( void ) const { return (last-first);}
};
//
///---------------------------------------------------------------------------
/// @struct token
/// @brief pair of iterators. This struct is used by several sort algorithms
/// @tparam iter_t : iterator to the elements to sort
/// @remarks
//----------------------------------------------------------------------------
template <class iter_t>
struct token_level
{   //----------- variables -------------
    iter_t first ;
    iter_t last ;
    uint32_t level ;

    //------------------- Functions -------------------
    token_level( void){} ;
    token_level ( iter_t it_f, iter_t it_l, int32_t lv)
                 :first (it_f),last(it_l),level(lv) {  };
    bool   	valid 		( void ) const { return ((last-first)>= 0 ); };
    size_t 	size		( void ) const { return (last-first);}
};
//
//****************************************************************************
};//    End namespace algorithm
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
