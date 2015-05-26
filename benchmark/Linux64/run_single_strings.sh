echo $(date) > single_strings.txt
./file_generator input.bin 100000000
./single_strings 1 >> single_strings.txt
./single_strings 2 >> single_strings.txt
./single_strings 3 >> single_strings.txt
./single_strings 4 >> single_strings.txt
./single_strings 5 >> single_strings.txt
./single_strings 6 >> single_strings.txt
rm input.bin
echo $(date)>> single_strings.txt
