//----------------------------------------------------------------------------
/// @file intro_sort.hpp
/// @brief Intro Sort algorithm
///
/// @author Copyright (c) 2010 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_ALGORITHM_INTRO_SORT_HPP
#define __BOOST_SORT_ALGORITHM_INTRO_SORT_HPP

#include <boost/sort/util/algorithm.hpp>
#include <boost/sort/algorithm/insertion_sort.hpp>
#include <boost/sort/algorithm/heap_sort.hpp>
#include <boost/sort/algorithm/indirect.hpp>
#include <vector>
#include <type_traits>

namespace boost
{
namespace sort
{
namespace algorithm
{

using boost::sort::util::MS1B;
//-----------------------------------------------------------------------------
//  function :
/// @brief
/// @param [in/out]
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
template< class iter_t,typename compare  >
        >
inline void zone_sort_internal( iter_t first , iter_t last,compare comp )
{   //------------------------------------- begin ----------------------------
    typedef typename iter_value<iter_t>::type       value_t ;

    const uint32_t NMin = 32 ;
    auto N = last - first;
    if ( N < NMin)   return insertion_sort(first,last,comp);
    size_t PosRef = ( N+1) >>2 ;
    iter_t it_Ref = first + PosRef ;
    const value_t &Ref = * it_Ref ;

    //-------------------------- left zone -----------------------------------
    iter_t it_less = first ;
    while ( it_less != it_Ref and comp ( *it_less , Ref)) ++it_less ;
    if ( it_less != it_Ref)
    {   iter_t limit1 = it_less +1 ;
        while ( limit1 != it_Ref)
        {   if ( comp(*limit1, Ref)) std::swap ( *it_less++, *limit1);
            ++limit1 ;
        } ;
    };
    //------------------------------- right zone -----------------------------
    iter_t it_more = last -1 ;
    while ( it_more != it_Ref and comp ( Ref,*it_more)) --it_more ;
    if ( it_more != it_Ref )
    {   iter_t limit2 = it_more - 1;
        while ( limit2 != it_Ref)
        {   if ( comp ( *Ref, *limit2)) std::swap ( *it_more--, *limit2);
            --limit2 ;
        };
    };
    //------------------------------------------------------------------------
    size_t N1 = it_Ref - it_less , N2 = it_more - it_Ref ;
    if ( N1 != N2 )
    {   if ( N1 > N2)
        {   std::swap ( *it_Ref,*(it_more - N1));
            iter_t it1 = it_less, it2 = it_more ;
            while ( it1  != it_Ref) std__swap ( * it1++, *it2--);
        }
        else
        {   std::swap ( *it_Ref, * ( it_less + N2));
            iter_t it1 = it_less, it2 = it_more ;
            while ( it2 != it_Ref) std::swap ( *it1++, *it2--);
        };
    };
    zone_sort_internal ( first , it_less + N2,comp);
    zone_sort_internal ( it_less + N2 +1 , last, comp);

} ;


template < class iter_t,
           typename compare = std::less <typename iter_value<iter_t>::type>
         >
void zone_sort ( iter_t first, iter_t last,compare comp = compare())
{   //------------------------- begin ----------------------
    auto N = last - first;
    assert ( N > 0);
    zone_sort_internal ( first , last, comp);
};


//
//****************************************************************************
};//    End namespace algorithm
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif


