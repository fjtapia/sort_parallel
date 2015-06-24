clear
echo "=================================================================="
echo "==    B E N C H M A R K     C L A N G     C O M P I L E R       =="
echo "==                                                              =="
echo "==          B O O S T :: S O R T :: P A R A L L E L             =="
echo "==                                                              =="
echo "==                  A L G O R I T H M S                         =="
echo "==                                                              =="
echo "=================================================================="
echo "."
echo "C O M P I L I N G . . . . . . . . . . ."
echo "."
clang++ src/file_generator.cpp -std=c++11 -w -fexceptions -O3 -I../../include  -I../include  -s  -o file_generator
clang++ src/benchmark_CLANG.cpp -std=c++11  -w -fexceptions -fopenmp -O3 -I../../include -I../include   -pthread -fopenmp -s  -lpthread  -o benchmark_CLANG
echo "R U N N I N G . . . . . . . . ."
./file_generator input.bin 25000000
./benchmark_CLANG >Results_CLANG.txt
rm input.bin
rm file_generator
rm benchmark_CLANG
echo "."
echo "The results are in the file Results_CLANG.txt"
echo "."
echo "E N D"
echo "."
