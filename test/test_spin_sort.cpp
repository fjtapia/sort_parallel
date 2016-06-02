//----------------------------------------------------------------------------
/// @file test_spin_sort.cpp
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
#define __DEBUG_SORT 1
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/sort/parallel/detail/spin_sort.hpp>
#include <vector>
#include <algorithm>

using boost::sort::parallel::detail::spin_sort ;
using boost::sort::parallel::detail::range_sort ;
using boost::sort::parallel::detail::util::range ;


void test2 ();
void test3 ();
void test4 ();
void test5 ();
void test6 ();
void test7 ();


struct xk
{   unsigned tail : 3 ;
    unsigned num  :24 ;
    bool operator < ( xk A) const
    {   return ( num < A.num);
    };
};

void test1()
{	std::mt19937_64 my_rand(0);
	typedef typename std::vector<uint64_t>::iterator iter_t ;
	typedef std::less<uint64_t>                      compare ;
	typedef range <iter_t>                           range_it ;

	std::vector<uint64_t> V1, V2, V3 ;
	uint32_t NELEM  = 0 ;

	//------------------------------------------------------------------------
	NELEM = 40 ;
	for ( uint32_t i =0 ; i < NELEM ; ++i)
	{	V1.push_back ( my_rand()%10000 );
	};
	V2 = V1 ;
	range_it R2 ( V2.begin() , V2.end());

	V3.resize (NELEM, 0);
	range_it RAux ( V3.begin() , V3.end());
	range_sort( R2, RAux,compare() ,1);

	std::sort ( V1.begin() , V1.end() );

	for ( uint32_t i = 0 ; i < NELEM ; ++i)
	{	BOOST_CHECK ( V1[i] == V3[i]) ;
	};

	//------------------------------------------------------------------------
	V1.clear() ;
	V2.clear() ;
	V3.clear() ;
	NELEM = 75 ;

	for ( uint32_t i =0 ; i < NELEM ; ++i)
	{	V1.push_back ( my_rand()%10000 );
	};
	V2 = V1 ;
	R2 = range_it( V2.begin() , V2.end());

	V3.resize (NELEM, 0);
	RAux = range_it( V3.begin() , V3.end());
	range_sort( RAux,R2,compare() ,2);
	std::sort ( V1.begin() , V1.end() );

	for ( uint32_t i = 0 ; i < NELEM ; ++i)
	{	BOOST_CHECK ( V1[i] == V2[i]);
	};

	//------------------------------------------------------------------------
	V1.clear() ;
	V2.clear() ;
	V3.clear() ;
	NELEM = 200 ;

	for ( uint32_t i =0 ; i < NELEM ; ++i)
	{	V1.push_back ( my_rand()%10000 );
	};
	V2 = V1 ;
	R2 = range_it( V2.begin() , V2.end());

	V3.resize (NELEM, 0);
	RAux = range_it( V3.begin() , V3.end());

	range_sort( R2,RAux,compare() ,3);

	std::sort ( V1.begin() , V1.end() );

	for ( uint32_t i = 0 ; i < NELEM ; ++i)
	{	BOOST_CHECK ( V1[i] == V3[i]) ;
	};
	//------------------------------------------------------------------------
	V1.clear() ;
	V2.clear() ;
	V3.clear() ;
	NELEM = 400 ;

	for ( uint32_t i =0 ; i < NELEM ; ++i)
	{	V1.push_back ( my_rand()%10000 );
	};
	V2 = V1 ;
	R2 = range_it( V2.begin() , V2.end());

	V3.resize (NELEM, 0);
	RAux = range_it( V3.begin() , V3.end());

	range_sort( RAux,R2,compare() ,4);
	std::sort ( V1.begin() , V1.end() );

	for ( uint32_t i = 0 ; i < NELEM ; ++i)
	{	BOOST_CHECK ( V1[i] == V2[i]) ;
	};
};

void test2 ()
{   //---------------------------- Inicio --------------------------------
    uint64_t  V1[300] ;
    typedef std::less<uint64_t> compare_t ;
    compare_t comp ;

    for ( uint32_t i =0 ; i < 200 ; ++i) V1[i]= i ;
    //std::cout<<"Smart Merge Sort Sorted, not repeated ---------------\n";
    spin_sort<uint64_t*, compare_t> ( &V1[0] , &V1[200], comp );
    for ( unsigned i = 1 ; i < 200 ; i ++ )
    {	BOOST_CHECK ( V1[i-1] <= V1[i] ) ;
    };

    for ( uint32_t i =0 ; i < 200 ; ++i) V1[i] = 199-i ;
    //std::cout<<"Smart Merge Sort Reverse sorted , not repeated ---------------\n";
    spin_sort <uint64_t*, compare_t> ( &V1[0] , &V1[200], comp );
    for ( unsigned i = 1 ; i < 200 ; i ++ )
    {	BOOST_CHECK ( V1[i-1] <= V1[i] ) ;
    };

    for ( uint32_t i =0 ; i < 300 ; ++i) V1[i] = 299-i ;
    //std::cout<<"Smart Merge Sort Reverse sorted , not repeated ---------------\n";
    spin_sort <uint64_t*, compare_t> ( &V1[0] , &V1[300], comp );
    for ( unsigned i = 1 ; i < 300 ; i ++ )
    {	BOOST_CHECK ( V1[i-1] <= V1[i] )  ;
    };

    for ( uint32_t i =0 ; i < 300 ; ++i) V1[i] = 88 ;
    //std::cout<<"Smart Merge Sort Equals ---------------\n";
    spin_sort<uint64_t*, compare_t> ( &V1[0] , &V1[300],comp );
    for ( unsigned i = 1 ; i < 300 ; i ++ )
    {	BOOST_CHECK ( V1[i-1] <= V1[i] ) ;
    };
};

void test3 ( )
{   //---------------------------------- begin ---------------------------
  typedef typename std::vector< xk>::iterator iter_t ;
    typedef std::less<xk> compare_t ;
    std::mt19937_64 my_rand(0);


    const uint32_t NMAX = 500000 ;

    //------------------------------------------------------------------------
    std::vector< xk> V1, V2, V3;
    V1.reserve ( NMAX);
    for ( uint32_t i = 0 ; i < 8 ; ++i)
    {   for ( uint32_t k =0 ; k < NMAX; ++k)
        {   uint32_t NM = my_rand() ;
            xk G ;
            G.num = NM >> 3 ;
            G.tail = i ;
            V1.push_back(G);
        };
    };
    V3 =V2 = V1 ;
    spin_sort <iter_t, compare_t > ( V1.begin() , V1.end(), compare_t());
    std::stable_sort( V2.begin() , V2.end() );

    //------------------------------------------------------------------------
    //            Comprobation
    //------------------------------------------------------------------------
    BOOST_CHECK ( V1.size() == V2.size()) ;
    for ( uint32_t i = 0 ; i < V1.size() ; ++i)
    {   BOOST_CHECK ( V1[i].num == V2[i].num and V1[i].tail == V2[i].tail);
    };

};

void test4(void)
{   //----------------------------- begin-------------------------------------
    typedef std::less<uint64_t> compare_t ;
    typedef typename std::vector<uint64_t>::iterator  iter_t ;

    const uint32_t NElem = 500000 ;
    std::vector<uint64_t>  V1 ;
    std::mt19937_64 my_rand(0);
    compare_t comp ;

    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back(my_rand() %NElem) ;
    //std::cout<<"spin_sort unsorted ---------------\n";
    spin_sort <iter_t, compare_t>(  V1.begin() , V1.end(), comp );
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	BOOST_CHECK ( V1[i-1] <= V1[i] )  ;
    };

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back (i );
    //std::cout<<"spin_sort sorted ---------------\n";
    spin_sort <iter_t, compare_t>(  V1.begin() , V1.end(), comp );
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	BOOST_CHECK ( V1[i-1] <= V1[i] ) ;
    };

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back( NElem-i) ;
    //std::cout<<"spin_sort reverse sorted ---------------\n";
    spin_sort <iter_t, compare_t>(  V1.begin() , V1.end(), comp );
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	BOOST_CHECK ( V1[i-1] <= V1[i] ) ;
    };

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back( 1000) ;
    //std::cout<<"spin_sort equals ---------------\n";
    spin_sort <iter_t, compare_t>(  V1.begin() , V1.end(), comp );
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	BOOST_CHECK ( V1[i-1] == V1[i] ) ;
    };
};

void test5 ( void)
{   //---------------------------------------- begin--------------------------
    typedef std::vector<uint64_t >::iterator iter_t ;
    typedef std::less<uint64_t>				 compare ;

    const uint32_t KMax = 500000 ;

    std::vector<uint64_t > K,M ;
    std::mt19937_64 my_rand(0);
    compare  comp ;

    for (uint32_t i =0 ; i < KMax ; ++i ) K.push_back( my_rand());
    M = K ;
    //std::cout<<"spin_sort - random elements ---------------\n";
    uint64_t * Ptr = std::get_temporary_buffer<uint64_t>(KMax >>1).first ;
    if ( Ptr == nullptr) throw std::bad_alloc() ;
    range<uint64_t*> Rbuf (Ptr , Ptr + (KMax >>1) );

    spin_sort<iter_t,compare> (K.begin(),K.end(), comp ,Rbuf);

    std::return_temporary_buffer ( Ptr) ;

    std::stable_sort ( M.begin() , M.end() , comp);
    for ( unsigned i = 0 ; i < KMax ; i ++ )
      BOOST_CHECK ( M[i] == K[i] );
};

void test6 ( void)
{   //------------------------------------ begin -----------------------------
    typedef std::vector<uint64_t >::iterator iter_t ;
    typedef std::less<uint64_t>				 compare_t ;
    std::vector <uint64_t > V ;

    for ( uint32_t i =0 ; i < 2083333 ; ++i)  V.push_back ( i);
    spin_sort<iter_t, compare_t> ( V.begin() , V.end(), compare_t ());
    for ( uint32_t i =0 ; i < V.size() ; ++i)
    {   BOOST_CHECK ( V[i] == i)  ;
    };
};

void test7 ( void)
{   //---------------------- begin ------------------------------------
    typedef typename std::vector<uint64_t>::iterator iter_t ;
    typedef std::less<uint64_t>				 compare_t ;

    compare_t comp ;
    const uint32_t NELEM = 416667 ;
    const uint32_t N1 = ( NELEM +1)/2 ;
    std::vector <uint64_t> A ;

    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 0);
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 0);

    spin_sort <iter_t, compare_t>( A.begin() + 1000,
    		                       A.begin() + (1000+NELEM),comp);

    for ( iter_t it =A.begin() + 1000 ; it != A.begin() + (1000+NELEM) ; ++it)
    {   BOOST_CHECK ( (*(it-1)) <= (*it)) ;
    };
    BOOST_CHECK (A[998] == 0 and A[999] == 0 and A[1000+NELEM] == 0
    		     and A[1001+NELEM] == 0);

    //------------------------------------------------------------------------
    A.clear() ;
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 999999999);
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 999999999);

    spin_sort <iter_t, compare_t>( A.begin() + 1000,
    		                       A.begin() + (1000+NELEM), comp);

    for ( iter_t it =A.begin() + 1001 ; it != A.begin() + (1000+NELEM) ; ++it)
    {   BOOST_CHECK ( (*(it-1)) <= (*it));
    };
    BOOST_CHECK (A[998] == 999999999 and A[999] == 999999999 and
        A[1000+NELEM] == 999999999 and A[1001+NELEM] == 999999999);

    //------------------------------------------------------------------------
    std::vector<uint64_t> B ( N1 + 2000 , 0);

    A.clear() ;
    range<uint64_t*> Rbuf ( &B[1000], (&B[1000])+ N1  );
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);

    spin_sort<iter_t, compare_t > ( A.begin(), A.end(), comp, Rbuf);
    for ( iter_t it =A.begin() +1 ; it != A.end() ; ++it)
    {   if ( (*(it-1)) > (*it))  std::cout<<"error 1\n";
    };
    BOOST_CHECK (B[998] == 0 and B[999] == 0 and B[1000+N1] == 0
    		     and B[1001+N1] == 0);

    for ( uint32_t i =0 ; i < B.size() ; ++i)B[i] = 999999999 ;
    A.clear() ;
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);
    spin_sort<iter_t, compare_t > (A.begin(), A.end(), comp, Rbuf);

    for ( iter_t it =A.begin() +1 ; it != A.end() ; ++it)
    {   BOOST_CHECK ( (*(it-1)) <= (*it)) ;
    };
    BOOST_CHECK (B[998] == 999999999 and B[999] == 999999999 and
                 B[1000+N1] == 999999999 and B[1001+N1] == 999999999);
};

int test_main( int, char*[] )
{   //-------------- begin------------
   test1() ;
    test2() ;
    test3() ;
    test4() ;
    test5() ;
    test6() ;
    test7() ;
    return 0 ;
};



