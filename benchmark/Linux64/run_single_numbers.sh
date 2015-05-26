echo $(date) > single_numbers.txt
./file_generator input.bin 100000000
./single_numbers 1 >> single_numbers.txt
./single_numbers 2 >> single_numbers.txt
./single_numbers 3 >> single_numbers.txt
./single_numbers 4 >> single_numbers.txt
./single_numbers 5 >> single_numbers.txt
./single_numbers 6 >> single_numbers.txt
rm input.bin
echo $(date) >> single_numbers.txt
