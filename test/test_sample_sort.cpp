//----------------------------------------------------------------------------
/// @file test_sample_sort.cpp
/// @brief
///
/// @author Copyright (c) 2010 2015 Francisco Jos� Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <boost/test/included/test_exec_monitor.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/sort/parallel/detail/sample_sort.hpp>

#include <vector>
#include <algorithm>

namespace bs_algo = boost::sort::parallel::detail;
namespace bs_util = boost::sort::parallel::detail::util ;


std::mt19937_64 my_rand(0);

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
std::ostream & operator <<( std::ostream & salida, const xk &s)
{   salida<<"["<<s.num<<" : "<<s.tail<<"] ";
    return salida ;
};

void test3 ( )
{   //---------------------------------- begin ---------------------------
	typedef typename std::vector< xk>::iterator iter_t;
	typedef std::less<xk>						compare ;

    std::mt19937_64 my_rand(0);

    const uint32_t NMAX = 500000 ;

    //------------------------------------------------------------------------
    std::vector< xk> V1, V2;
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
    V2 = V1 ;
    bs_algo::sample_sort<iter_t, compare> ( V1.begin() , V1.end());
    std::stable_sort( V2.begin() , V2.end() );

    //------------------------------------------------------------------------
    //            Comprobation
    //------------------------------------------------------------------------
    BOOST_CHECK  ( V1.size() == V2.size()) ;
    for ( uint32_t i = 0 ; i < V1.size() ; ++i)
    {   BOOST_CHECK ( V1[i].num == V2[i].num and V1[i].tail == V2[i].tail);
    };


};

void test4(void)
{   //----------------------------- begin-------------------------------------
	typedef typename std::vector<uint64_t>::iterator iter_t ;
	typedef std::less<uint64_t>                      compare ;

    const uint32_t NElem = 500000 ;
    std::vector<uint64_t>  V1 ;
    std::mt19937_64 my_rand(0);

    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back(my_rand() %NElem) ;
    //std::cout<<"sample_sort unsorted ---------------\n";
    bs_algo::sample_sort<iter_t, compare > (  V1.begin() , V1.end() );
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	BOOST_CHECK ( V1[i-1] <= V1[i] ) ;
    };

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back (i );
    //std::cout<<"sample_sort sorted ---------------\n";
    bs_algo::sample_sort <iter_t, compare >(  V1.begin() , V1.end() );
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	BOOST_CHECK ( V1[i-1] <= V1[i] ) ;
    };

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back( NElem-i) ;
    //std::cout<<"sample_sort reverse sorted ---------------\n";
    bs_algo::sample_sort<iter_t, compare > (  V1.begin() , V1.end() );
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	BOOST_CHECK ( V1[i-1] <= V1[i] )  ;
    };

    V1.clear() ;
    for ( uint32_t i =0 ; i < NElem ; ++i) V1.push_back( 1000) ;
    //std::cout<<"sample_sort equals ---------------\n";
    bs_algo::sample_sort <iter_t, compare >(  V1.begin() , V1.end() );
    for ( unsigned i = 1 ; i < NElem ; i ++ )
    {	BOOST_CHECK ( V1[i-1] == V1[i] ) ;
    };
};

void test5 ( void)
{   //---------------------------------------- begin--------------------------
	typedef typename std::vector<uint64_t>::iterator iter_t ;
	typedef std::less<uint64_t>                      compare ;
    const uint32_t KMax = 500000 ;

    std::vector<uint64_t > K,M ;
    std::mt19937_64 my_rand(0);
    std::less<uint64_t>  comp ;

    for (uint32_t i =0 ; i < KMax ; ++i ) K.push_back( my_rand());
    M = K ;
    //std::cout<<"sample_sort - random elements ---------------\n";
    uint64_t * Ptr = std::get_temporary_buffer<uint64_t>(KMax ).first ;
    if ( Ptr == nullptr) throw std::bad_alloc() ;
    bs_util::range<uint64_t*> Rbuf ( Ptr , Ptr + KMax);

    bs_algo::sample_sort <iter_t,compare >(K.begin(),K.end(),comp,
    		                       std::thread::hardware_concurrency(), Rbuf);

    std::return_temporary_buffer ( Ptr) ;

    std::stable_sort ( M.begin() , M.end() , comp);
    for ( unsigned i = 0 ; i < KMax ; i ++ )
    	BOOST_CHECK ( M[i] == K[i] )  ;
};

void test6 ( void)
{   //------------------------------------ begin -----------------------------
	typedef typename std::vector<uint64_t>::iterator iter_t ;
	typedef std::less<uint64_t>                      compare ;
    std::vector <uint64_t > V ;

    for ( uint32_t i =0 ; i < 2083333 ; ++i)  V.push_back ( i);
    bs_algo::sample_sort<iter_t, compare> ( V.begin() , V.end() );
    for ( uint32_t i =0 ; i < V.size() ; ++i)
    {   BOOST_CHECK ( V[i] == i) ;
    };
};

void test7 ( void)
{   //---------------------- begin ------------------------------------
    typedef typename std::vector<uint64_t>::iterator iter_t ;
	typedef std::less<uint64_t>                      compare ;

    const uint32_t NELEM = 416667 ;
    //const uint32_t N1 = ( NELEM +1)/2 ;
    std::vector <uint64_t> A ;

    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 0);
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 0);

    bs_algo::sample_sort<iter_t, compare> ( A.begin() + 1000,
    		                                A.begin() + (1000+NELEM));

    for ( iter_t it =A.begin() + 1000 ; it != A.begin() + (1000+NELEM) ; ++it)
    {   BOOST_CHECK ( (*(it-1)) <= (*it)) ;
    };

    BOOST_CHECK ( A[998] == 0 and A[999] == 0 and A[1000+NELEM] == 0
    		      and A[1001+NELEM] == 0);

    //------------------------------------------------------------------------
    A.clear() ;
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 999999999);
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);
    for ( uint32_t i =0 ; i < 1000 ; ++i) A.push_back ( 999999999);

    bs_algo::sample_sort<iter_t, compare> ( A.begin() + 1000,
    		                                A.begin() + (1000+NELEM));

    for ( iter_t it =A.begin() + 1001 ; it != A.begin() + (1000+NELEM) ; ++it)
    {   BOOST_CHECK ( (*(it-1)) <= (*it));
    };
    BOOST_CHECK (A[998] == 999999999 and A[999] == 999999999 and
        A[1000+NELEM] == 999999999 and A[1001+NELEM] == 999999999);


    //------------------------------------------------------------------------
    std::vector<uint64_t> B ( NELEM + 2000 , 0);

    A.clear() ;
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);

    bs_util::range<uint64_t*> Rbuf (&B[1000], (&B[1000] )+NELEM );
    bs_algo::sample_sort<iter_t,compare> (A.begin() , A.end() ,
                                     std::less<uint64_t>(),
					                 std::thread::hardware_concurrency(),Rbuf);

    for ( iter_t it =A.begin() +1 ; it != A.end() ; ++it)
    {   BOOST_CHECK ( (*(it-1)) <= (*it));
    };
    BOOST_CHECK (B[998] == 0 and B[999] == 0 and
    		     B[1000+NELEM] == 0 and B[1001+NELEM] == 0);

    //------------------------------------------------------------------------
    for ( uint32_t i =0 ; i < B.size() ; ++i)B[i] = 999999999 ;
    A.clear() ;
    for ( uint32_t i =0 ; i < NELEM ; ++i) A.push_back ( NELEM - i);

    bs_algo::sample_sort<iter_t,std::less<uint64_t> > ( A.begin(), A.end(),
                                        std::less<uint64_t>(),
					                    std::thread::hardware_concurrency(),
									    Rbuf);

    for ( iter_t it =A.begin() +1 ; it != A.end() ; ++it)
    {   if ( (*(it-1)) > (*it)) std::cout<<"error 2\n";
    };
    BOOST_CHECK (B[998] == 999999999 and B[999] == 999999999 and
        B[1000+NELEM] == 999999999 and B[1001+NELEM] == 999999999);

    //------------------------------------------------------------------------
};

int test_main( int, char*[] )
{   //-------------- begin------------
    test3() ;
    test4() ;
    test5() ;
    test6() ;
    test7() ;
    return 0 ;
};
