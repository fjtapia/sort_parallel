echo $(date) > single_objects.txt
./file_generator input.bin 100000000
./single_objects 1 >> single_objects.txt
./single_objects 2 >> single_objects.txt
./single_objects 3 >> single_objects.txt
./single_objects 4 >> single_objects.txt
./single_objects 5 >> single_objects.txt
rm input.bin
echo $(date) >> single_objects.txt
