clear
echo "=================================================================="
echo "==     B E N C H M A R K     G C C      C O M P I L E R         =="
echo "==                                                              =="
echo "==          B O O S T :: S O R T :: P A R A L L E L             =="
echo "==                                                              =="
echo "==                  A L G O R I T H M S                         =="
echo "==                                                              =="
echo "=================================================================="
echo "."
echo "C O M P I L I N G . . . . . . . . . . ."
echo "."
g++ src/file_generator.cpp -std=c++11 -w -fexceptions -O3 -I../../include  -I../include  -s  -o file_generator
g++ src/benchmark_GCC_TBB_BOOST.cpp -std=c++11 -D_GLIBCXX_USE_SCHED_YIELD -w -fexceptions -fopenmp -O3 -I../../include -I../include  -pthread -fopenmp -s  -lpthread -ltbb  -o benchmark_GCC_TBB_BOOST
echo "R U N N I N G . . . . . . . . ."
./file_generator input.bin 25000000
./benchmark_GCC_TBB_BOOST >Results_GCC.txt
rm input.bin
rm file_generator
rm benchmark_GCC_TBB_BOOST
echo "."
echo "The results are in the file Results_GCC.txt"
echo "."
echo "E N D"
echo "."
