echo $(date) > parallel_numbers.txt
./file_generator input.bin 100000000
./parallel_numbers 1 >> parallel_numbers.txt
./parallel_numbers 2 >> parallel_numbers.txt
./parallel_numbers 3 >> parallel_numbers.txt
./parallel_numbers 4 >> parallel_numbers.txt
./parallel_numbers 5 >> parallel_numbers.txt
./parallel_numbers 6 >> parallel_numbers.txt
./parallel_numbers 7 >> parallel_numbers.txt
rm input.bin
echo $(date) >> parallel_numbers.txt
