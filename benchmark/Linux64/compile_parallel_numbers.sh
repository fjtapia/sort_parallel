g++ ./parallel_numbers.cpp -std=c++11 -W -fexceptions -fopenmp -O3 -I../../include -I../include -pthread -fopenmp -s  -lpthread -ltbb  -o parallel_numbers
