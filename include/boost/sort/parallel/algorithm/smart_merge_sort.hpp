//----------------------------------------------------------------------------
/// @file smart_merge_sort.hpp
/// @brief Quick Sort algorithm
///
/// @author Copyright (c) 2010 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#ifndef __BOOST_SORT_ALGORITHM_SMART_MERGE_SORT_HPP
#define __BOOST_SORT_ALGORITHM_SMART_MERGE_SORT_HPP

#include <functional>
#include <memory>
#include <cstdlib>
#include <type_traits>
#include <vector>
#include <boost/sort/parallel/util/util_iterator.hpp>
#include <boost/sort/parallel/util/algorithm.hpp>
#include <boost/sort/parallel/algorithm/indirect.hpp>
#include <boost/sort/parallel/algorithm/insertion_sort.hpp>
#include <boost/sort/parallel/util/buffer.hpp>

namespace boost
{
namespace sort
{
namespace parallel
{
namespace algorithm
{
//****************************************************************************
//                 NAMESPACES AND USING SENTENCES
//****************************************************************************
using namespace boost::sort::parallel::util ;

template <class iter1_t  , class iter2_t, class compare>
inline bool full_sort_internal (iter1_t it1 , iter2_t it2, size_t N ,
                                compare comp, uint32_t level );

template <class iter_t  , class value_t, class compare>
inline bool full_sort_even ( iter_t it , value_t* pit, size_t N ,
                             compare comp , uint32_t level );

template <class iter_t  , class value_t, class compare>
inline bool full_sort_odd  ( iter_t it , value_t* pit, size_t N ,
                             compare comp , uint32_t level );


//
//---------------------------------------------------------------------------
//  function : full_sort_internal
/// @brief internal sort and merge function
/// @tparam iter1_t : first type of iterators
/// @tparam iter2_t : second type of iterators
/// @tparam compare : object for to compare the elements pointed by iter1_t
///                   and iter2_t itertors ( Must be of the same type)
/// @param [in] it1 : iterator to the first element of the first range
/// @param [in] it2 : iterator to the first element of the second range
/// @param [in] N : Number of elements of the ranges
/// @param [in] comp : object for to compare elements
/// @param [in] level : Level of deep in the calling from the first call
/// @exception none
/// @return true if the number of levels is even, false if it's odd
/// @remarks
//---------------------------------------------------------------------------
template <class iter1_t  , class iter2_t, class compare>
inline bool full_sort_internal (iter1_t it1 , iter2_t it2, size_t N ,
                                compare comp, uint32_t level )
{   //---------------------------- begin ------------------------------------
    size_t N1 = (N+1) >>1;
    size_t N2 = N - N1 ;

    if ( level == 0)
    {   insertion_sort (it1, it1 + N1,comp );
        insertion_sort (it1+N1 , it1 + N, comp );
    }
    else
    {   full_sort_internal ( it1 , it2 , N1,comp,level-1);
        full_sort_internal ( it1+N1, it2 +N1 , N2,comp, level-1);
    };
    if ( (level &1) == 0)  full_merge( it1, it1+N1, it1+N1, it1+N,it2, comp);
    else                   full_merge( it2 ,it2+N1, it2+N1, it2+N,it1, comp);
    return ( (level&1)!=0);
};
//
//---------------------------------------------------------------------------
//  function : full_sort
/// @brief internal sort and merge function
/// @tparam iter1_t : first type of iterators
/// @tparam iter2_t : second type of iterators
/// @tparam compare : object for to compare the elements pointed by iter1_t
///                   and iter2_t itertors ( Must be of the same type)
/// @param [in] it1 : iterator to the first element of the first range
/// @param [in] it2 : iterator to the first element of the second range
/// @param [in] N : Number of elements of the ranges
/// @param [in] comp : object for to compare elements
/// @exception none
/// @return true if the number of levels is even, false if it's odd
/// @remarks
//---------------------------------------------------------------------------
template < class iter1_t, class iter2_t,  typename compare >
bool full_sort (iter1_t it1 , iter2_t it2, size_t N , compare comp)
{   //------------------------------------------------------------------------
    typedef typename iter_value<iter1_t>::type value1_t ;
    typedef typename iter_value<iter2_t>::type value2_t ;
    static_assert ( std::is_same<value1_t, value2_t>::value ,
                   "Incompatible iterators\n");
    //------------------------------------------------------------------------
    const size_t Sort_min = 36 ;
    if ( N < 2 ) return true ;
    if (N <= Sort_min)
    {   insertion_sort ( it1, it1 + N, comp);
        return true ;
    };
    //------------------------------------------------------------------------
    // When the level is even, the merge finish in RX, when is odd in R
    // When the level is 0, sort the parts after merge, if not invoque to
    // sort_internal
    //------------------------------------------------------------------------
    uint32_t NLevel = NBits ( (N -1)/Sort_min);
    return  ( full_sort_internal ( it1, it2, N, comp, NLevel -1) );
};
//
//---------------------------------------------------------------------------
//  function : full_sort_even
/// @brief internal sort and merge function on even levels
/// @tparam iter_t : type of iterators
/// @tparam value_t: elements pointed by the iterators
/// @tparam compare : object for to compare the elements pointed by iter1_t
//
/// @param [in] it : iterator  to the first element of the first buffer
/// @param [in] pit : pointer to the memory area of the second buffer
/// @param [in] N : number of elements of the buffers
/// @param [in] comp : object for to compare two objects
/// @param [in] level of deep from the first call
/// @exception none
/// @return true if the number of levels is even, false if it's odd
/// @remarks
//---------------------------------------------------------------------------
template <class iter_t  , class value_t, class compare>
inline bool full_sort_even ( iter_t it , value_t* pit, size_t N ,
                             compare comp , uint32_t level )
{   //---------------------------- begin ------------------------------------
    typedef typename iter_value<iter_t>::type value1_t ;
    static_assert ( std::is_same<value1_t, value_t>::value ,
                   "Incompatible iterators\n");
    //------------------------------------------------------------------------
    size_t N1 = (N+1)>>1 ;
    size_t N2 = N - N1  ;

    if ( level == 0)
    {   insertion_sort (it, it +N1, comp );
        insertion_sort (it+N1, it+N,comp );
        uninit_full_merge( it, it+N1, it+N1, it+N, pit,comp);
    }
    else
    {   full_sort_odd (it , pit, N1 ,comp,level-1);
        full_sort_odd (it+N1, pit+N1, N2 ,comp, level-1);
        full_merge(  it, it+N1, it+N1, it+N,pit,comp);
    };
    return ( (level&1)!=0);
};
//
//---------------------------------------------------------------------------
//  function : full_sort_odd
/// @brief internal sort and merge function on odd levels
/// @tparam iter_t : type of iterators
/// @tparam value_t: elements pointed by the iterators
/// @tparam compare : object for to compare the elements pointed by iter1_t
//
/// @param [in] it : iterator  to the first element of the first buffer
/// @param [in] pit : pointer to the memory area of the second buffer
/// @param [in] N : number of elements of the buffers
/// @param [in] comp : object for to compare two objects
/// @param [in] level of deep from the first call
/// @exception none
/// @return true if the number of levels is even, false if it's odd
/// @remarks
//---------------------------------------------------------------------------
template <class iter_t  , class value_t, class compare>
inline bool full_sort_odd  ( iter_t it , value_t* pit, size_t N ,
                             compare comp , uint32_t level )
{   //---------------------------- begin ------------------------------------
    typedef typename iter_value<iter_t>::type value1_t ;
    static_assert ( std::is_same<value1_t, value_t>::value ,
                   "Incompatible iterators\n");
    //------------------------------------------------------------------------
    size_t N1 = (N+1)>>1 ;
    size_t N2 = N - N1  ;

    full_sort_even (it , pit, N1 ,comp,level-1);
    full_sort_even (it+N1, pit+N1, N2 ,comp, level-1);
    full_merge(  pit, pit+N1, pit+N1, pit+N,it,comp);
    return ( (level&1)!=0);
};
//
//-----------------------------------------------------------------------------
//  function : full_uninit_sort
/// @brief Done a merge_sort with an auxiliary memory of the same size than the
///        data.
/// @tparam iter_t : type of iterators
/// @tparam value_t: elements pointed by the iterators
/// @tparam compare : object for to compare the elements pointed by iter1_t
/// @param [in] it : iterator to the first element of the first buffer
/// @param [in] pit : pointer to the first element of the second buffer
/// @param [in] N : number of elements of the buffer
/// @param [in] comp : comparison object
/// @exception none
/// @return true if the number of levels is even, false if it's odd
/// @remarks
//-----------------------------------------------------------------------------
template < class iter_t, class value_t,  typename compare >
bool full_uninit_sort (iter_t it, value_t *pit, size_t N, compare comp)
{   //------------------------------------------------------------------------
    typedef typename iter_value<iter_t>::type value1_t ;
    static_assert ( std::is_same<value1_t, value_t>::value ,
                   "Incompatible iterators\n");
    //------------------------------------------------------------------------
    const size_t Sort_min = 36 ;
    if ( N < 2 ) return true ;
    if ( N <= Sort_min)
    {	insertion_sort (it , it + N, comp);
        uninit_move_buf (pit, it , it + N );
        return false ;
    };
    //------------------------------------------------------------------------
    // When the level is even, the merge finish in RX, when is odd in R
    // When the level is 0, sort the parts after merge, if not invoque to
    // sort_internal
    //------------------------------------------------------------------------
    uint32_t NLevel = NBits ( (N -1)/Sort_min);

    return ( (NLevel &1) != 0 )?full_sort_even ( it, pit, N, comp, NLevel-1):
                                full_sort_odd  ( it, pit, N, comp, NLevel-1);
};

//---------------------------------------------------------------------------
/// @struct smart_merge_sort_tag
/// @brief this is a struct for to do a stable sort exception safe
/// @tparam iter_t : iterator to the elements
/// @tparam compare : object for to compare the elements pointed by iter_t
/// @remarks
//----------------------------------------------------------------------------
template < class iter_t,
           typename compare=std::less<typename iter_value<iter_t>::type >
         >
struct smart_merge_sort_tag
{
//****************************************************************************
//               DEFINITIONS AND CONSTANTS
//****************************************************************************
typedef typename iter_value<iter_t>::type value_t ;
static const uint32_t Sort_min = 36 ;

//****************************************************************************
//                      VARIABLES
//****************************************************************************
value_t *Ptr ;
size_t NPtr ;
bool construct = false , owner = false ;


//-----------------------------------------------------------------------------
//  function : smart_merge_sort_tag
/// @brief constructo of the struct
/// @param [in] first : iterator to the first element of the range
/// @param [in] last : iterator to next element after the last of the range
/// @param [in] comp : object for to compare two elements
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
smart_merge_sort_tag ( iter_t first, iter_t last, compare comp)
                      :Ptr(nullptr),NPtr(0),construct(false),owner(false)
{   //-------------------------- begin -------------------------------------
    auto NR = last- first ;
    assert ( NR >=0);
    size_t N = size_t ( NR);
    owner= construct= false ;

    NPtr = (N +1 ) >>1 ;
    size_t N1 = NPtr, N2 = N - NPtr ;

    if ( N <= ( Sort_min))
    {   insertion_sort (first, last, comp);
        return ;
    };
    //------------------- check if sort --------------------------------------
    bool SW = true ;
    for ( iter_t it1 = first, it2 = first+1 ;
          it2 != last and (SW = not comp(*it2,*it1));it1 = it2++);
    if (SW) return ;

    Ptr = std::get_temporary_buffer<value_t>(NPtr).first ;
    if ( Ptr == nullptr) throw std::bad_alloc() ;
    owner = true ;

    //------------------------------------------------------------------------
    //                  Process
    //------------------------------------------------------------------------
    if ( full_uninit_sort ( first,Ptr, N1, comp))
        move_buf ( Ptr, first , first + N1);
    construct = true ;

    if ( not full_sort ( first + N1, first, N2,comp))
        move_buf ( first+ N1, first , first + N2);
    half_merge (Ptr, Ptr+ N1, first+N1, first +N, first, comp );
};
//-----------------------------------------------------------------------------
//  function : smart_merge_sort_tag
/// @brief constructo of the struct
/// @param [in] first : iterator to the first element of the range
/// @param [in] last : iterator to next element after the last of the range
/// @param [in] comp : object for to compare two elements
/// @param [in] bfirst : pointer to the auxiliary memory used in the sorting
/// @param [in] Nb : size un number of objects of the memory pointed by bfirst
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
smart_merge_sort_tag ( iter_t first, iter_t last, compare comp,
                       value_t * bfirst, size_t Nb):Ptr(bfirst), NPtr(Nb),
                       construct(false),owner(false)
{   //-------------------------- begin -------------------------------------
    auto NR = last- first ;
    assert ( NR >=0);
    size_t N = size_t ( NR);
    owner= construct= false ;

    NPtr = (N +1 ) >>1 ;
    assert ( Nb >= NPtr) ;
    size_t N1 = NPtr, N2 = N - NPtr ;

    if ( N <= ( Sort_min))
    {   insertion_sort (first, last, comp);
        return ;
    };
    //------------------------------------------------------------------------
    //                  Process
    //------------------------------------------------------------------------
    if ( full_uninit_sort ( first,Ptr, N1, comp))
        move_buf ( Ptr, first , first + N1);
    construct = true ;

    if ( not full_sort ( first + N1, first, N2,comp))
        move_buf ( first+ N1, first , first + N2);
    half_merge (Ptr, Ptr+ N1, first+N1, first +N, first, comp );
};
//-----------------------------------------------------------------------------
//  function :~smart_merge_sort_tag
/// @brief destructor of the struct. Deallocate all the data structure used
///        in the sorting
/// @exception
/// @return
/// @remarks
//-----------------------------------------------------------------------------
~smart_merge_sort_tag ( void)
{   //----------------------------------- begin -------------------------
    if ( construct)
    {   destroy ( Ptr, Ptr +NPtr);
        construct = false ;
    };
    if ( owner and Ptr != nullptr) std::return_temporary_buffer ( Ptr) ;
};

//
//----------------------------------------------------------------------------
};//        End of class merge_sort_tag
//----------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//  function : smart_merge_sort
/// @brief Smart Merge Sort algorithm ( stable sort)
/// @tparam iter_t : iterator to the elements
/// @tparam compare : object for to compare the elements pointed by iter_t
/// @param [in] first : first iterator to the elements to sort
/// @param [in] last : last iterator to the elements to sort
/// @param [in] comp : object for to compare two elements
/// @exception
/// @return
/// @remarks
//---------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less<typename iter_value<iter_t>::type> >
void smart_merge_sort (iter_t first, iter_t last, compare comp = compare() )
{   //----------------------------- begin ----------------------------
    smart_merge_sort_tag<iter_t,compare> ( first, last , comp);
};
//
//---------------------------------------------------------------------------
//  function : smart_merge_sort
/// @brief Smart Merge Sort algorithm ( stable sort)
/// @tparam iter_t : iterator to the elements
/// @tparam value_t : objects pointed by the iter_t iterators
/// @tparam compare : object for to compare the elements pointed by iter_t
/// @param [in] first : first iterator to the elements to sort
/// @param [in] last : last iterator to the elements to sort
/// @param [in] comp : object for to compare two elements
/// @param [in] P1 : pointer of a data area with at least (N+1)/2 elements
/// @param [in] NP1 : capacity of the memory area pointed by P1
/// @exception
/// @return
/// @remarks
//---------------------------------------------------------------------------
template < class iter_t,
          typename value_t= typename iter_value<iter_t>::type,
          typename compare = std::less<value_t>  >
void smart_merge_sort ( iter_t first, iter_t last, compare comp,
                        value_t* bfirst, size_t Nb)

{   //------------------------------- begin--------------------------
    smart_merge_sort_tag<iter_t,compare> ( first, last , comp,bfirst,Nb );
};
//############################################################################
//                                                                          ##
//                I N D I R E C T     F U N C T I O N S                     ##
//                                                                          ##
//############################################################################
//
//---------------------------------------------------------------------------
//  function : indirect_smart_merge_sort
/// @brief Indirect sort using the smart_merge_sort algorithm ( stable sort)
/// @tparam iter_t : iterator to the elements
/// @tparam compare : object for to compare the elements pointed by iter_t
/// @param [in] first : first iterator to the elements to sort
/// @param [in] last : last iterator to the elements to sort
/// @param [in] comp : object for to compare two elements
/// @exception
/// @return
/// @remarks
//---------------------------------------------------------------------------
template < class iter_t,
           typename compare = std::less<typename iter_value<iter_t>::type>  >
void indirect_smart_merge_sort ( iter_t first, iter_t last,
                                 compare comp = compare() )
{   //------------------------------- begin--------------------------
    typedef less_ptr_no_null <iter_t, compare>      compare_ptr ;

    std::vector<iter_t> VP ;
    create_index ( first , last , VP);
    smart_merge_sort  ( VP.begin() , VP.end(), compare_ptr(comp) );
    sort_index ( first , VP) ;
};


//****************************************************************************
};//    End namespace algorithm
};//    End namespace parallel
};//    End namespace sort
};//    End namespace boost
//****************************************************************************
//
#endif
