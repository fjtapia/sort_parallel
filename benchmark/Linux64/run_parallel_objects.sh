echo $(date) > parallel_objects.txt
./file_generator input.bin 100000000
./parallel_objects 1 >> parallel_objects.txt
./parallel_objects 2 >> parallel_objects.txt
./parallel_objects 3 >> parallel_objects.txt
./parallel_objects 4 >> parallel_objects.txt
./parallel_objects 5 >> parallel_objects.txt
./parallel_objects 6 >> parallel_objects.txt
./parallel_objects 7 >> parallel_objects.txt
rm input.bin
echo $(date) >> parallel_objects.txt
