//----------------------------------------------------------------------------
/// @file benchmark_VC++.cpp
/// @brief Benchmark of several sort methods with different elements
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
///         This program use for comparison purposes only the TimSort
///         implementation, https://github.com/gfx/cpp-TimSort which license is
///         https://github.com/gfx/cpp-TimSort/blob/master/LICENSE
///         This program use for comparison purposes, the Threading Building
///         Blocks which license is the GNU General Public License, version 2
///         as  published  by  the  Free Software Foundation.
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------
#include <ppl.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <string>
#include <file_vector.hpp>

#include <boost/sort/parallel/sort.hpp>
#include <boost/sort/parallel/util/time_measure.hpp>
#include <file_vector.hpp>
#include <boost/sort/parallel/util/atomic.hpp>
#include "int_array.hpp"
#include <boost/sort/spreadsort/spreadsort.hpp>


#define NELEM 25000000
#define NMAXSTRING 2000000
using namespace std ;
namespace bsp = boost::sort::parallel ;
namespace bsp_util = bsp::util ;

using bsp_util::time_point ;
using bsp_util::now;
using bsp_util::subtract_time ;
using bsp_util::fill_vector_uint64;
using bsp_util::write_file_uint64;
using bsp::NThread ;

void Generator_sorted(void );
void Generator_uint64(void );
void Generator_string(void) ;

template <class IA>
void Generator (uint64_t N );

template <class IA>
int Prueba  ( std::vector <IA> & A);

template <class IA>
int Prueba_spreadsort  (const std::vector <IA> & B );

template <class IA>
int Prueba_parallel_radix_sort(const std::vector <IA> & B);

int main (int argc, char *argv[] )
{   //------------------------------ Inicio ----------------------------------
	wcout << endl;
    wcout<<"****************************************************************\n";
    wcout<<"**                                                            **\n";
    wcout<<"**         B O O S T :: S O R T :: P A R A L L E L            **\n";
    wcout<<"**                                                            **\n";
    wcout<<"**               F A S T  B E N C H M A R K                   **\n";
    wcout<<"**                                                            **\n";
    wcout<<"****************************************************************\n";

    wcout.flush();
    system ( "wmic cpu get description>>salida1.txt");
	wcout.flush();
    system ( "wmic cpu get name >>salida1.txt");
	wcout.flush();
    system ( "wmic cpu get numberofcores>>salida1.txt");
	wcout.flush();
    system ( "wmic cpu get numberoflogicalprocessors>>salida1.txt");
    wcout.flush();
	system("type salida1.txt ");
	system("del salida1.txt");
	wcout << endl ;
    //------------------------------------------------------------------------
    // Execution with different object format
    //------------------------------------------------------------------------
    Generator_sorted() ;
    Generator_uint64() ;
    Generator_string () ;
    Generator< int_array<1> >   ( NELEM   );
    Generator< int_array<2> >   ( NELEM>>1);
    Generator< int_array<4> >   ( NELEM>>2);
    Generator< int_array<8> >   ( NELEM>>3);
    Generator< int_array<16> >  ( NELEM>>4);
    Generator< int_array<32> >  ( NELEM>>5);
    Generator< int_array<64> >  ( NELEM>>6);
    return 0 ;
}
void Generator_sorted(void )
{   //---------------------------- begin--------------------------------------
    vector<uint64_t> A  ;
    A.reserve ( NELEM) ;
    wcout<<"  "<<NELEM<<" uint64_t elements already sorted\n" ;
    wcout<<"=================================================\n";
    A.clear();
    for ( size_t i =0 ; i < NELEM ; ++i)
        A.push_back( i );
    Prueba(A) ;
    Prueba_spreadsort( A);
    Prueba_parallel_radix_sort(A);
    wcout<<std::endl ;
}
void Generator_uint64(void )
{   //---------------------------- begin--------------------------------------
    vector<uint64_t> A  ;
    A.reserve ( NELEM) ;
    wcout<<"  "<< NELEM<<" uint64_t elements randomly filled\n" ;
    wcout<<"=================================================\n";
    A.clear();
    if ( fill_vector_uint64("input.bin", A, NELEM) != 0)
    {   std::wcout<<"Error in the input file\n";
        return ;
    };
    Prueba(A) ;
    Prueba_spreadsort( A);
    Prueba_parallel_radix_sort(A);
    wcout<<std::endl ;
}
void Generator_string(void)
{   //------------------------------- begin ----------------------------------
    wcout<<"  "<< NMAXSTRING<<" strings randomly filled\n" ;
    wcout<<"===============================================\n";
    std::vector <std::string> A ;
    A.reserve ( NMAXSTRING);
    A.clear();
    if ( bsp_util::fill_vector_string("input.bin", A, NMAXSTRING) != 0)
    {   std::wcout<<"Error in the input file\n";
        return ;
    };
    Prueba(A) ;
    Prueba_spreadsort( A);
    wcout<<std::endl ;
};


template <class IA>
void Generator (uint64_t N )
{   //------------------------------- begin ----------------------------------
    bsp::util::uint64_file_generator gen ( "input.bin");
    vector<IA> A ;
   	A.reserve ( N);
   	IA Aux(0);

    //------------------------ Inicio -----------------------------
    wcout<<N<<" elements of size "<<sizeof (IA)<<" randomly filled \n";
    wcout<<"=============================================\n";
    gen.reset() ;
    A.clear();
    for ( uint32_t i = 0 ; i < N ; i ++)
        A.emplace_back(IA::generate(Aux,gen)) ;
    Prueba(A) ;
    wcout<<std::endl ;
};
template <class IA>
int Prueba  ( std::vector <IA> & B )
{   
	double duracion ;
	time_point start, finish;
	std::vector <IA> A ( B);
    
	
	A = B ;
    wcout<<"VC++ std::sort               : ";
    start= now() ;
    std::sort (A.begin() , A.end()  );
    finish = now() ;
    duracion = subtract_time(finish ,start) ;
    wcout<<duracion<<" secs\n";

    
    A = B ;
    wcout<<"Boost introsort              : ";
    start= now() ;
    boost::sort::parallel::introsort (A.begin() , A.end() );
    finish = now() ;
    duracion = subtract_time(finish ,start) ;
    wcout<<duracion<<" secs\n";

    A = B ;
    wcout<<"VC++ std::stable_sort        : ";
    start= now() ;
    std::stable_sort (A.begin() , A.end()  );
    finish = now() ;
    duracion = subtract_time(finish ,start) ;
    wcout<<duracion<<" secs\n";

    A = B ;
    wcout<<"Boost smart_merge_sort       : ";
    start= now() ;
    boost::sort::parallel::smart_merge_sort (A.begin() , A.end()  );
    finish = now() ;
    duracion = subtract_time(finish ,start) ;
    wcout<<duracion<<" secs\n";

    A = B;
    wcout<<"PPL parallel sort            : ";
    start= now() ;
	concurrency::parallel_sort(A.begin(), A.end());
	finish = now();
	duracion = subtract_time(finish, start);
	wcout << duracion << " secs\n";

    A = B ;
	wcout<<"PPL parallel buffered sort   : ";
	start = now();
	concurrency::parallel_buffered_sort(A.begin(), A.end());
	finish = now();
	duracion = subtract_time(finish, start);
	wcout << duracion << " secs\n";

    A = B ;
    wcout<<"Boost parallel introsort     : ";
    start= now() ;
    bsp::parallel_introsort (A.begin() , A.end() );
    finish = now() ;
    duracion = subtract_time(finish ,start) ;
    wcout<<duracion<<" secs\n";

    A = B ;
    wcout<<"Boost parallel stable sort   : ";
    start= now() ;
    bsp::parallel_stable_sort (A.begin() , A.end() ,NThread() );
    finish = now() ;
    duracion = subtract_time(finish ,start) ;
    wcout<<duracion<<" secs\n";

    A = B ;
    wcout<<"Boost sample sort            : ";
    start= now() ;
    bsp::sample_sort (A.begin() , A.end() ,NThread() );
    finish = now() ;
    duracion = subtract_time(finish ,start) ;
    wcout<<duracion<<" secs\n";

    return 0 ;
};
template <class IA>
int Prueba_spreadsort  ( const std::vector <IA> & B )
{   //---------------------------- begin --------------------------------
	double duracion ;
	time_point start, finish;
	std::vector <IA> A ( B);
    std::less<IA>  comp ;

	A = B ;
    wcout<<"Boost spreadsort             : ";
    start= now() ;
    boost::sort::spreadsort::spreadsort (A.begin() , A.end()  );
    finish = now() ;
    duracion = subtract_time(finish ,start) ;
    wcout<<duracion<<" secs\n";
	return 0;
};
template <class IA>
int Prueba_parallel_radix_sort  ( const std::vector <IA> & B )
{   //---------------------------- begin --------------------------------
	double duracion ;
	time_point start, finish;
	std::vector <IA> A ( B);
    std::less<IA>  comp ;

	A = B ;
    wcout<<"PPL parallel radix sort		 : ";
    start= now() ;
    concurrency::parallel_radixsort(A.begin(), A.end());
    finish = now() ;
    duracion = subtract_time(finish ,start) ;
    wcout<<duracion<<" secs\n";
	return 0;
};
