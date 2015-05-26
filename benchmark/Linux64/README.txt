For to compile and run these benchmarks need to have installed Threading Building
Blocks ( see in your repository), because these benchmarks compare the algorithms
developed for Boost, with the TBB and GCC parallel algorithms.
You need too a version of Boost. (The Spreadsort use several files of other Boost libraries)

All the benchmarks create a file of 800 M with random information, needed 
for to fill the vectors to sort. It is deleted when finish the benchmark.

Open a terminal in this folder and execute ./compile_all.sh
(Appear many warnings in the TBB code, don't worry about it)

For to execute a program you have a shell with their name. If generate a text file
with all the results

If you execute ./run_all.sh execute all the benchmarks. Be careful with 
this option  because the time needed for to run all the test is around
90 mins depending of the machine.

The compiler shell scripts run wel with GCC 4.8 and 4.9 . If you need to 
compile with 4.7 you need to include in the shellscript of compilation 
the option of define _GLIBCXX_USE_SCHED_YIELD
