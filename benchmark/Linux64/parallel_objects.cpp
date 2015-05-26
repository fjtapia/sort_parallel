//----------------------------------------------------------------------------
/// @file benchmark_sort_04.cpp
/// @brief Benchmark of several sort methods with different uint64_t numbers
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <file_vector.hpp>

#include <parallel/basic_iterator.h>
#include <parallel/features.h>
#include <parallel/parallel.h>
#include <parallel/algorithm>
#include <omp.h>

#include "tbb/tbb_stddef.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_sort.h"
#include <parallel_stable_sort/tbb-lowlevel/parallel_stable_sort.h>

#include <boost/sort/parallel/sort.hpp>
#include <boost/sort/parallel/util/time_measure.hpp>
#include "int_array.hpp"

#define NELEM 100000000
using namespace std ;
namespace bs_parallel = boost::sort::parallel ;
namespace bs_algo = boost::sort::parallel::algorithm ;

using boost::sort::parallel::util::time_point ;
using boost::sort::parallel::util::now;
using boost::sort::parallel::util::subtract_time ;
using boost::sort::parallel::util::fill_vector_uint64;
using boost::sort::parallel::util::write_file_uint64;
using boost::sort::parallel::NThread ;

//std::mt19937_64 my_rand(0);
template <class IA> void Generator (uint64_t N , int option);
int Prueba ( int option) ;
vector<uint64_t> A  ;

int main (int argc, char *argv[] )
{   //------------------------------ Inicio ----------------------------------
    int option  =0 ;
    if ( argc > 1) option = atoi ( argv[1]);
    if ( option == 0 )
    {   //----------------------------- menu ---------------------------------
        std::cout<<std::endl<<std::endl ;
        std::cout<<std::endl<<std::endl ;
        std::cout<<"\t      M E N U \n";
        std::cout<<"\t    =============\n\n\n";
        std::cout<<"\t 1.- GCC   parallel sort \n";
        std::cout<<"\t 2.- tbb   parallel sort \n";
        std::cout<<"\t 3.- boost parallel_sort \n";
        std::cout<<"\t 4.- GCC   parallel stable sort \n";
        std::cout<<"\t 5.- boost parallel stable sort \n";
        std::cout<<"\t 6.- boost sample sort \n";
        std::cout<<"\t 7.- tbb   lowlevel parallel stable sort \n";

        std::cout<<std::endl<<std::endl ;
        std::cout<<"\t Select option -> ";
        std::cin>>option ;
        cout<<"\n";
    };
    if ( option < 1 or option > 7 ) return 0 ;
    //------------------------------------------------------------------------
    //        LOAD THE DATA FROM THE FILE
    //------------------------------------------------------------------------
	A.reserve ( NELEM);
    cout<<"\n";
    //------------------------ Inicio -----------------------------
    switch ( option)
    {   case 1 :
            cout<<"GCC parallel sort (not stable sort)\n";
            cout<<"-------------------------------------\n";
            break ;
        case 2 :
            cout<<"tbb parallel_sort (not stable sort)\n";
            cout<<"-------------------------------------\n";
            break ;
        case 3 :
            cout<<"boost parallel sort (non stable sort)\n";
            cout<<"---------------------------------------\n";
            break ;
        case 4 :
            cout<<"GCC parallel stable sort (stable sort)\n";
            cout<<"----------------------------------------\n";
            break ;
        case 5 :
            cout<<"boost parallel stable sort (stable sort)\n";
            cout<<"-------------------------------------------\n";
            break ;
        case 6 :
            cout<<"boost sample sort (stable sort)\n";
            cout<<"---------------------------------\n";
            break ;
        case 7 :
            cout<<"TBB lowlevel parallel stable sort (stable sort)\n";
            cout<<"--------------------------------------------------\n";
            break ;
    } ;
    cout<<endl;
    //------------------------------------------------------------------------
    // Execution with different object format
    //------------------------------------------------------------------------
    Generator< int_array<1> > ( NELEM, option);
    Generator< int_array<2> > ( NELEM>>1, option);
    Generator< int_array<4> > ( NELEM>>2, option);
    Generator< int_array<6> > ( NELEM /6, option);
    Generator< int_array<8> > ( NELEM>>3, option);
    Generator< int_array<16> > ( NELEM>>4, option);
    Generator< int_array<32> > ( NELEM>>5, option);
    //Generator< int_array<64> > ( NELEM>>6, option);
    //Generator< int_array<128> > ( NELEM>>7, option);
    return 0 ;
}


template <class IA>
void Generator (uint64_t N , int option)
{   //------------------------------- begin ----------------------------------
    bs_parallel::util::uint64_file_generator gen ( "input.bin");
    vector<IA> A ;
   	A.reserve ( N);
   	IA Aux(0);

    //------------------------ Inicio -----------------------------
    cout<<"Sort of N = "<<N<<" elements of size "<<sizeof (IA)<<"\n";
    cout<<"--------------------------------------------\n";

    cout<<"Sorted            : ";
    A.clear() ;
    for ( uint32_t i = 0 ; i < N ; i ++)
        A.push_back( N +i) ;
    Prueba(A, option) ;

    cout<<"Reverse sorted    : ";
    A.clear() ;
    for ( uint32_t i = 0 ; i < N ; i ++)
        A.push_back( (N<<1) -i) ;
    Prueba(A, option) ;

    cout<<"Random            : ";
    gen.reset() ;
    A.clear();
    for ( uint32_t i = 0 ; i < N ; i ++)
        A.emplace_back(IA::generate(Aux,gen)) ;
    Prueba(A,option) ;

    cout<<"Random % 50000000 : ";
    A.clear() ;
    gen.reset() ;
    gen.set_max_val (N/2 );
    for ( uint32_t i = 0 ; i < N ; i ++)
        A.emplace_back( IA::generate(Aux,gen)) ;
    Prueba(A, option) ;

    cout<<"Random % 10000    : ";
    A.clear() ;
    gen.reset() ;
    gen.set_max_val (10000 );
    for ( uint32_t i = 0 ; i < N ; i ++)
        A.emplace_back( IA::generate(Aux,gen)) ;
    Prueba(A, option) ;

    cout<<"Equal elements    : ";
    A.clear() ;
    for ( uint32_t i = 0 ; i < N  ; i ++) A.push_back( N);
    Prueba(A, option) ;

    cout<<endl ;

};
template <class IA>
int Prueba  ( std::vector <IA> & A,  int option)
{   //---------------------------- Inicio --------------------------------
	double duracion ;
	time_point start, finish;
    std::less<IA> comp ;
	//--------------------------------------------------------------------
    switch ( option )
    {
    case 1:
        //----------------- __gnu_parallel::sort ------------------------";
        start= now() ;
        __gnu_parallel::sort (A.begin() , A.end());
        finish = now() ;
        duracion = subtract_time(finish ,start) ;
        cout<<duracion<<" secs\n";
        for ( unsigned i = 0 ; i < (A.size() -1) ; i ++ )
        {	if (A[i+1] < A[i]  ) cout<<"Error in Pos :"<<i ;
        };
        break ;

    case 2 :
        //------------------- tbb::parallel_sort -----------------------------
        start= now() ;
        tbb::parallel_sort (A.begin() , A.end()  );
        finish = now() ;
        duracion = subtract_time(finish ,start) ;
        cout<<duracion<<" secs\n";
        for ( unsigned i = 0 ; i < (A.size() -1) ; i ++ )
        {	if (A[i+1] < A[i]  ) cout<<"Error in Pos :"<<i ;
        };
        break ;

    case 3 :
        //--------------------- boost::sort::parallel_sort -------------------
        start= now() ;
        //bs_algo::indirect_parallel_sort (A.begin() , A.end() );
        bs_parallel::parallel_introsort (A.begin() , A.end() );
        finish = now() ;
        duracion = subtract_time(finish ,start) ;
        cout<<duracion<<" secs\n";
        for ( unsigned i = 0 ; i < (A.size() -1) ; i ++ )
        {	if (A[i+1] < A[i]  ) cout<<"Error in Pos :"<<i ;
        };
        break ;

    case 4 :
        //------------------- __gnu_parallel::stable_sort --------------------
        start= now() ;
        __gnu_parallel::stable_sort (A.begin() , A.end()  );
        finish = now() ;
        duracion = subtract_time(finish ,start) ;
        cout<<duracion<<" secs\n";
        for ( unsigned i = 0 ; i < (A.size() -1) ; i ++ )
        {	if (A[i+1] < A[i]  ) cout<<"Error in Pos :"<<i ;
        };
        break ;

    case 5 :
        //--------------------- boost::sort::parallel_stable_sort ------------
        start= now() ;
        bs_parallel::parallel_stable_sort (A.begin() , A.end() ,NThread() );
        finish = now() ;
        duracion = subtract_time(finish ,start) ;
        cout<<duracion<<" secs\n";
        for ( unsigned i = 0 ; i < (A.size() -1) ; i ++ )
        {	if ( A[i+1] < A[i] ) cout<<"Error Pos :"<<i ;
        };
        break ;

    case 6 :
        //--------------------- boost::sort::sample_sort ---------------------
        start= now() ;
        //bs_algo::indirect_sample_sort (A.begin() , A.end() ,NThread() );
        bs_parallel::sample_sort (A.begin() , A.end() ,NThread() );
        finish = now() ;
        duracion = subtract_time(finish ,start) ;
        cout<<duracion<<" secs\n";
        for ( unsigned i = 0 ; i < (A.size() -1) ; i ++ )
        {	if ( A[i+1] < A[i] ) cout<<"Error Pos :"<<i ;
        };
        break ;

    case 7 :
        //--------------------- tbb lowlevel parallel_stable_sort ------------
        start= now() ;
        pss_tbbl::parallel_stable_sort (A.begin() , A.end(),comp );
        finish = now() ;
        duracion = subtract_time(finish ,start) ;
        cout<<duracion<<" secs\n";
        for ( unsigned i = 0 ; i < (A.size() -1) ; i ++ )
        {	if ( A[i+1] < A[i] ) cout<<"Error Pos :"<<i ;
        };
        break ;

        cout<<endl;
    };
    return 0 ;
};
