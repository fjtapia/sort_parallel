
<h1>Boost Sort Parallel</h1>
<h3> <a href="https://github.com/fjtapia/sort_parallel">https://github.com/fjtapia/sort_parallel</a> </h3>

<h3>PRELIMINARY</h3>

The **Boost Sort Parallel Library**, is part of the Boost Sort Library, created by Steven Ross.



<h3>DESCRIPTION</h3>

This library provide **stable and not stable** sorting algorithms, in **single thread and parallel** versions.

These algorithms do not use any other library or utility. To **compile and run** only need a **C++11 compliant compiler.**

The algorithms **use a comparison object**, in the same way than the sort algorithms of the standard library. If you don't define, by default is std::less object, which uses internally the operator < internally for comparisons.

The algorithms are **exception safe**, it means,  the exceptions generated by the algorithms guarantee the integrity of the objects to sort , but not their relative order. If the exception is generated inside the objects (in the move or in the copy constructor.. ) the results can be unpredictable.

**This library is include only**. Don't need to link with any static or dynamic library. Don't have  dependence of any other boost files, variables or libraries or any other external libraries. To use, only need a to include the files of the boost/sort/parallel folder, any more.

This table provide you a brief description of the sort algorithms of the library.

| Algorithm | Parallel | Stable | Additional memory | Best, average, and worst case |
| --- | --- | --- | --- | --- |
| sort | no | no | Log N  | NLogN, NLogN, NLogN |
| stable\_sort | no | yes| N / 2 | NLogN, NLogN, NLogN   |
| parallel\_sort | yes | no | block_size\* num_threads | NLogN, NLogN, NLogN  |
| parallel\_stable\_sort| yes | yes | N / 2 | NLogN, NLogN, NLogN   |
| sample\_sort | yes | yes | N  | NLogN, NLogN, NLogN  |


The block_size is an internal parameter of the algorithm, which  in order to achieve the highest speed, change according the size of the objects to sort according the next table. The **strings** use a block_size of 128.

| object size (bytes) |1 - 15| 16 - 31 | 32 - 63 | 64 - 127 |	128 - 255 | 256 - 511 |	512 - |
| --- | --- | --- | --- | --- | --- | --- | --- |
| block_size |4096 |2048 | 1024| 768 | 512 | 256 | 128 |




<h3>PRESENT PERSPECTIVE</h3>


In the parallel sorting algorithms, we can find two categories of algorithms.

**SUBDIVISION ALGORITHMS**

Filter the data and generate two or more parts. Each part obtained is filtered and divided by other thread, until the size of the data to sort is smaller than a predefined size, then it is sorted by a single thread. The algorithm most frecuently used in the filter and sorting is quicksort.

These algorithms are  **fast with a small number of threads**, but with a great number of HW threads , show their lacks. Examples of this category are 

 - Intel Threading Building Blocks (TBB) 
 - Microsoft PPL Parallel Sort.


**MERGING ALGORITHMS**

Divide the data in parts, and each part is sorted by a thread. When the parts are sorted, must merge them to obtain the final results. The problem of these algorithms is they need additional memory to the merge, and usually with the same size than the data.

With a small number of threads, have similar speed than the subdivision algorithms, but **with many threads they are  much more faster** . Examples of this category are :

 - GCC Parallel Sort (based on OpenMP)
 - Microsoft PPL Parallel Buffered Sort

<h3>NEW PARALLEL SORT ALGORITHM</h3>

This generate an **undesirable duality**. With a small number of threads use one algorithm, and with a big number use other. Due this, the SW designed for a small machine is inadequate for a big machine and vice versa. But the main problem, in the merging algorithms is the memory used, usually of the same size than the data.

This version have as novelty a **new parallel\_sort algorithm** *( internally named Block Indirect)*, created for processors connected with **shared memory**.

It is a hybrid algorithm. With small number of threads, it is a subdivision algorithm, but with many threads is a merging algorithms, which need a small  auxiliary memory ( block\_size * num\_threads).

The block_size is an internal parameter of the algorithm, which  in order to achieve the highest speed, change according the size of the objects to sort according the next table. The **strings** use a block_size of 128.


| object size (bytes) |1 - 15| 16 - 31 | 32 - 63 | 64 - 127 |	128 - 255 | 256 - 511 |	512 - |
| --- | --- | --- | --- | --- | --- | --- | --- |
| block_size |4096 |2048 | 1024| 768 | 512 | 256 | 128 |


This algorithm permit to eliminate the duality. When run in a machine with small number of threads have the performance of TBB, and when run in a machine with many threads, the same code have the performance of GCC Parallel Sort, with the **additional advantage** of the **small memory consumption**.

The algorithm use as **auxiliary memory a block_size elements** for each thread. The worst case is when have very big elements and many threads. With big elements (512 bytes), and 12 threads, The memory measured was:

| Algorithm | Memory used in MB |
| --- | --- |
| GCC Parallel Sort (OpenMP) | 1565 MB |
| Threading Building Blocks (TBB) | 783 MB |
| Block Indirect Sort | 812 MB |


*This **new parallel\_sort algorithm had been  created and implemented specifically for this library** by the author.*

*In the documentation of the project, you can find all the information about the algorithm ( description, characteristics, how to program,  and articles with the detailed description of the new algorithm)  [index.html](index.html)

*In the project you have a small benchmarks. If you want run in your machine the full benchmarks with non free SW, you have all the code, intructions and procedures in 
[(https://github.com/fjtapia/sort_parallel_benchmark)](https://github.com/fjtapia/sort_parallel_benchmark)

<h2>Installation </h2>
 * This library is **include only**.  
 * Don't need to link with any static or dynamic library.
 * Don't have  dependence of any other boost libraries or any other external libraries.
 * To use, only need include the files of the boost/sort/parallel folder.  
 * This library had been compiled successfully with the next compilers
 
| Compiler |
| --- |
| GCC  4.8, 4.9 , 5.2 |
| CLANG 3.6, 3.7, 3.8 |
| Visual C++ 2013, 2015 |



<h2>Author and Copyright</h2>

This library is integrated in the [Boost](http://www.boost.org) library, inside the [boost::sort library](http://www.boost.org/doc/libs/release/libs/sort),
with the spreadsort algorithms designed and implemented by Steven Ross.

You can find in  [https://github.com/fjtapia/sort_parallel](https://github.com/fjtapia/sort_parallel)

This algorithm had been ideate, designed and implemented beginning from zero. After read hundreds of articles and books, I didn't find any similar. If someone knows something about this or something similar, please, say me.

Anyway, the important is not the author, is provide a fast, robust, and easy to use algorithm to the community of programmers.


Copyright 2016  [Francisco Tapia *(fjtapia@gmail.com)* ](mail:fjtapia@gmail.com)
Distributed under the [Boost Software License, Version 1.0. ](http://www.boost.org/LICENSE_1_0.txt)  (See http://www.boost.org/LICENSE_1_0.txt)
