//----------------------------------------------------------------------------
/// @file indirect.hpp
/// @brief Indirect algorithm
///
/// @author Copyright (c) 2010 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_DETAIL_INDIRECT_HPP
#define __BOOST_SORT_PARALLEL_DETAIL_INDIRECT_HPP

#include <vector>
#include <type_traits>
#include <functional>
#include <iterator>
#include <boost/sort/parallel/detail/util/atomic.hpp>



namespace boost		{
namespace sort		{
namespace parallel	{
namespace detail	{

using std::iterator_traits ;
//
//##########################################################################
//                                                                        ##
//         S T R U C T     L E S S _ P T R _ N O _ N U L L                ##
//                                                                        ##
//##########################################################################
//
//---------------------------------------------------------------------------
/// @class less_ptr_no_null
///
/// @remarks this is the comparison object for pointers. Receive a object
///          for to compare the objects pointed. The pointers can't be nullptr
//---------------------------------------------------------------------------
template    <   class iter_t ,
                class comp_t
				=std::less<typename iterator_traits<iter_t>::value_type> >
struct less_ptr_no_null
{   //----------------------------- Variables -----------------------
    comp_t comp ;
    //----------------------------- Functions ----------------------
    inline less_ptr_no_null ( comp_t C1 = comp_t()):comp(C1){};
    inline bool operator ()( iter_t  T1,  iter_t  T2 ) const
    {   //-------------------- begin ------------------------------
        return  comp(*T1 ,*T2);
    };
};
//
//-----------------------------------------------------------------------------
//  function : create_index
/// @brief Create a index of iterators to the elements
/// @tparam iter_t : iterator to store in the index vector
/// @param [in] first : iterator to the first element of the range
/// @param [in] last : iterator to the element after the last of the range
/// @param [in/out] v_iter : vector where store the iterators of the index
//-----------------------------------------------------------------------------
template <class iter_t>
void create_index (iter_t first, iter_t last, std::vector<iter_t> &v_iter )
{   //----------------------------- begin -------------------------------
    auto nelem = last-first ;
    assert ( nelem >= 0 );
    v_iter.clear() ;
    v_iter.reserve ( nelem);
    for ( ; first != last ; ++first) v_iter.push_back( first);
};
//
//-----------------------------------------------------------------------------
//  function : sort_index
/// @brief sort the elements according of the sort of the index
/// @tparam iter_t : iterators of the index
/// @param [in] first : iterator to the first element of the data
/// @param [in] v_iter : vector sorted of the iterators
//-----------------------------------------------------------------------------
template <class iter_t>
void sort_index (iter_t first, std::vector<iter_t> &v_iter )
{   //-------------------------- begin -------------------------------------
    typedef typename iterator_traits<iter_t>::value_type       value_t ;
    size_t pos_dest  = 0 , pos_src =0 , pos_in_vector =0 , nelem = v_iter.size();
    iter_t it_dest, it_src ;
    while ( pos_in_vector < nelem )
    {   while  ( pos_in_vector < nelem and (size_t (v_iter[pos_in_vector]-first)) == pos_in_vector ) ++pos_in_vector;
        if ( pos_in_vector == nelem ) return ;
        pos_dest = pos_src = pos_in_vector ;
        it_dest = first + pos_dest  ;
        value_t Aux = std::move ( *it_dest);
        while ( (pos_src = (size_t (v_iter[pos_dest]-first)))!= pos_in_vector  )
        {   v_iter[pos_dest] = it_dest;
            it_src = first + pos_src ;
            *it_dest = std::move ( *it_src);
            it_dest = it_src ;
            pos_dest = pos_src ;
        };
        *it_dest = std::move ( Aux ) ;
        v_iter[pos_dest] = it_dest ;
        ++pos_in_vector ;
    };
};
//
//****************************************************************************
};//    End namespace detail
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
