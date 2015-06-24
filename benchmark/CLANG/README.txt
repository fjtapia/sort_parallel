README
============

The Boost Sort have two parts:
1.- The Spreadsort algorithms invented and implemented by Steven Ross 
    (approved and part of the Official release of Boost)
2.- The Parallel algorithms, implemented by Francisco Tapia.
    ( pending of the final approval) 

These benchmarks are for the parallel algorithms and compare the time spent 
sorting different data and different objects, with several sorting algorithms.

The algorithms included in Boost Parallel are :
    1. introsort ( single thread , not stable )
    2. smart_merge_sort ( single_thread, stable)
    3. parallel_introsort ( parallel , not stable )
    4. parallel_stable_sort ( parallel, stable)
    5. sample_sort( parallel, stable)

The algorithms involved in the comparison are 

STL
    1. std::sort ( single thread , not stable )
    2. std::stable_sort ( single thread , stable )

Timsort ( https://github.com/gfx/cpp-TimSort )
    1. Timsort ( single thread , stable )

TBB
    1. parallel_sort ( parallel, not stable)
    2. parallel_stable_sort ( parallel, stable , experimental)

Boost Spreadsort
    1. Spreadsort (single_thread , not stable)


The Sort parallel library is an include only library and don't need any 
other SW, file or library. Only a C++11 compiler.

The Spreadsort use others files of the Boost library. For to compile, 
you need to install the Boost library

The TBB algorithms need to install the Threading Building Blocks.

You have shell scripts for to compile and run the code. The compilation 
spend about two minutes, and the running about five minutes, depending of 
the speed of your computer. 

The result generated are in the file results_CLANG.txt

Depending of the SW installed on your computer, you must select an script 
or another.

run_CLANG.sh        = Boost Parallel + STL + Timsort
run_CLANG_BOOST.sh  = Boost Parallel + STL + Timsort + Spreadsort
run_CLANG_TBB.sh    = Boost Parallel + STL + Timsort + TBB
run_CLANG_TBB_BOOST = Boost Parallel + STL + Timsort + Spreadsort + TBB

The scripts compile the programs, generate a 200M data file, and run the 
benchmark. 
When finish, the programs and the data file are deleted, and you will
have the results in Results_CLANG.txt

A more detailed about the algorithms and the results obtained can be 
found in the documentation
