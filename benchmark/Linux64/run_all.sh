echo $(date)
lscpu > cpu.txt
./run_single_numbers.sh
./run_single_objects.sh
./run_single_strings.sh
./run_parallel_numbers.sh
./run_parallel_objects.sh
./run_parallel_strings.sh
echo $(date)

