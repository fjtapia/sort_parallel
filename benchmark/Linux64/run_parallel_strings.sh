echo $(date) > parallel_strings.txt
./file_generator input.bin 100000000
./parallel_strings 1 >> parallel_strings.txt
./parallel_strings 2 >> parallel_strings.txt
./parallel_strings 3 >> parallel_strings.txt
./parallel_strings 4 >> parallel_strings.txt
./parallel_strings 5 >> parallel_strings.txt
./parallel_strings 6 >> parallel_strings.txt
./parallel_strings 7 >> parallel_strings.txt
rm input.bin
echo $(date) >> parallel_strings.txt
