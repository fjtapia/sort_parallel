//----------------------------------------------------------------------------
/// @file spin_sort.hpp
/// @brief Spin Sort algorithm
///
/// @author Copyright (c) 2010 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_PARALLEL_ALGORITHM_SPIN_SORT_HPP
#define __BOOST_SORT_PARALLEL_ALGORITHM_SPIN_SORT_HPP


#include <functional>
#include <memory>
#include <cstdlib>
#include <type_traits>
#include <iterator>
#include <vector>
#include <boost/sort/parallel/detail/util/nbits.hpp>
#include <boost/sort/parallel/detail/util/compare_traits.hpp>
#include <boost/sort/parallel/detail/indirect.hpp>
#include <boost/sort/parallel/detail/insertion_sort.hpp>
#include <boost/sort/parallel/detail/util/range.hpp>

namespace boost		{
namespace sort		{
namespace parallel	{
namespace detail	{
//
//****************************************************************************
//                 NAMESPACES AND USING SENTENCES
//****************************************************************************
namespace su = boost::sort::parallel::detail::util ;
using su::range ;
using su::nbits64 ;
using std::iterator_traits ;

//-----------------------------------------------------------------------------
//  function : range_sort
/// @brief this function divide r_input in two parts, sort it ,and merge moving
///        the elements to range_buf
/// @param [in] r_input : range with the elements to sort
/// @param [in] range_buf : range with the elements sorted
/// @param [in] comp : object for to compare two elements
/// @param [in] level : when is 0, sort with the insertion_sort algorithm
///                     if not make a recursive call swaping the ranges
/// @return range with all the elements sorted and moved
//-----------------------------------------------------------------------------
template <class iter1_t  , class iter2_t, class compare>
void range_sort ( const range<iter1_t> &rng_a , const range<iter2_t> &rng_b,
                         compare comp, uint32_t level )
{   //---------------------------- begin ------------------------------------
	typedef range<iter1_t>				range_it1 ;
	typedef range<iter2_t>				range_it2 ;

	//assert ( rng_a.size() == rng_b.size() and level != 0 );

    size_t nelem1 = (rng_a.size() +1) >>1;
    //rng_b.last = rng_b.first + rng_a.size() ;

    range_it1 rng_a1 ( rng_a.first        , rng_a.first + nelem1),
    		  rng_a2 (rng_a.first + nelem1, rng_a.last          );

    if ( level < 2)
    {	insertion_sort (rng_a1.first, rng_a1.last,comp );
    	insertion_sort (rng_a2.first ,rng_a2.last, comp );
    }
    else
    {	range_sort (range_it2(rng_b.first, rng_b.first + nelem1),
    		        rng_a1 , comp , level - 1  ) ;
    	range_sort (range_it2 (rng_b.first + nelem1, rng_b.last ),
    			    rng_a2 , comp , level - 1  );
    };
    full_merge (rng_b, rng_a1, rng_a2,comp );
};

//---------------------------------------------------------------------------
/// @struct spin_sort
/// @brief this is a struct for to do a stable sort exception safe
/// @tparam Iter_t : iterator to the elements
/// @tparam Compare : object for to Compare the elements pointed by Iter_t
/// @remarks
//----------------------------------------------------------------------------
template < class Iter_t, typename Compare= util::compare_iter<Iter_t> >
class spin_sort
{
//****************************************************************************
//               DEFINITIONS AND CONSTANTS
//****************************************************************************
typedef typename iterator_traits<Iter_t>::value_type 	value_t ;
typedef range < Iter_t >								range_it ;
typedef range < value_t * >								range_buf;
static const uint32_t Sort_min = 36 ;

//****************************************************************************
//                      VARIABLES
//****************************************************************************
value_t *ptr ;
size_t nptr ;
bool construct = false , owner = false ;

//-----------------------------------------------------------------------------
//  function : spin_sort
/// @brief constructor of the struct
/// @param [in] R : range of elements to sort
/// @param [in] comp : object for to compare two elements
//-----------------------------------------------------------------------------
spin_sort (Iter_t first,Iter_t last,Compare comp,value_t* paux, size_t naux );

public:
//
//-----------------------------------------------------------------------------
//  function : spin_sort
/// @brief constructor of the struct
/// @param [in] r_input : range of elements to sort
/// @param [in] comp : object for to Compare two elements
//-----------------------------------------------------------------------------
spin_sort ( Iter_t first , Iter_t last , Compare comp = Compare()):
spin_sort ( first, last, comp, nullptr, 0 ) { } ;
//
//-----------------------------------------------------------------------------
//  function : spin_sort
/// @brief constructor of the struct
/// @param [in] r_input : range of elements to sort
/// @param [in] comp : object for to Compare two elements
/// @param [in] range_aux : range used as auxiliary memory
//-----------------------------------------------------------------------------
spin_sort (Iter_t first, Iter_t last , Compare comp, range_buf range_aux ):
spin_sort ( first, last, comp, range_aux.first , range_aux.size()) {};
//
//-----------------------------------------------------------------------------
//  function :~spin_sort
/// @brief destructor of the struct. Deallocate all the data structure used
///        in the sorting
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
~spin_sort ( void)
{   //----------------------------------- begin -------------------------
    if ( construct)
    {   destroy (range<value_t*>( ptr, ptr +nptr));
        construct = false ;
    };
    if ( owner and ptr != nullptr) std::return_temporary_buffer ( ptr) ;
};

//
//----------------------------------------------------------------------------
};//        End of class spin_sort
//----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  function : spin_sort
/// @brief constructor of the struct
/// @param [in] r_input : range of elements to sort
/// @param [in] comp : object for to Compare two elements
//-----------------------------------------------------------------------------
template < class Iter_t,  class Compare>
spin_sort<Iter_t, Compare>::
spin_sort (Iter_t first, Iter_t last, Compare comp,value_t * paux, size_t naux)
         : ptr ( paux ), nptr ( naux ) , construct ( false ) , owner ( false )
{   //-------------------------- begin -------------------------------------
	range<Iter_t> r_input ( first, last );
	assert ( r_input.valid());

    size_t nelem = r_input.size();
    owner= construct= false ;

    nptr = (nelem +1 ) >>1 ;
    size_t nelem_1 = nptr ;
	size_t nelem_2 = nelem - nelem_1 ;

    if ( nelem <= ( Sort_min << 1 ))
    {   insertion_sort (r_input.first, r_input.last, comp);
        return ;
    };
    //------------------- check if sort --------------------------------------
    bool sw = true ;
    for ( Iter_t it1 = r_input.first, it2 = r_input.first+1 ;
          it2 != r_input.last and (sw = not comp(*it2,*it1));it1 = it2++);
    if (sw) return ;

    if ( ptr == nullptr )
    {   ptr = std::get_temporary_buffer<value_t>(nptr).first ;
        if ( ptr == nullptr) throw std::bad_alloc() ;
        owner = true ;
    };
    range_buf  rng_buf ( ptr , (ptr + nptr));

    //------------------------------------------------------------------------
    //                  Process
    //------------------------------------------------------------------------
    uint32_t nlevel = nbits64 ( ((nelem +Sort_min -1)/Sort_min) - 1) -1;
    assert ( nlevel != 0);

	if ( (nlevel &1) == 1 )
	{	//---------------------------------------------------------------------
		// if the number of levels is odd, the data are in the first parameter
		// of range_sort, and the results appear in the second parameter
		//---------------------------------------------------------------------
		range_it rng_a1( first , first + nelem_2), rng_a2 ( first + nelem_2, last);
		rng_buf = uninit_move ( rng_buf , rng_a2) ;
		construct = true ;

		range_sort ( rng_buf, rng_a2, comp , nlevel) ;
		range_buf rng_bx( rng_buf.first , rng_buf.first + nelem_2);

		range_sort ( rng_a1, rng_bx , comp,nlevel) ;
		half_merge ( r_input, rng_bx, rng_a2, comp ) ;
	}
	else
	{	//--------------------------------------------------------------------
		// If the number of levels is even, the data are in the second
		// parameter of range_sort, and the results are in the same parameter
		//---------------------------------------------------------------------
		range_it rng_a1( first , first + nelem_1), rng_a2 ( first + nelem_1, last) ;
		rng_buf = uninit_move ( rng_buf, rng_a1) ;
		construct = true ;

		range_sort ( rng_a1, rng_buf , comp, nlevel);

		rng_a1.last = rng_a1.first + rng_a2.size() ;
		range_sort ( rng_a1, rng_a2 , comp, nlevel );
		half_merge ( r_input, rng_buf, rng_a2, comp ) ;
	};

};

//****************************************************************************
};//    End namespace algorithm
};//    End namespace parallel
};//    End namespace sort
};//    End namepspace boost
//****************************************************************************
//
#endif
