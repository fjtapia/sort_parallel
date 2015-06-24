For to execute the benchmark , yo need to compile the two programs  in this folder

file_generator.cpp
benchmark_VC++.cpp

It is important compile Benchamrk_VC++ with all the speed optimizations

After this execute

file_generator input.bin 25000000

This create a 200 M file with random data used by the benchmark_VC++ program

now execute 

benchmark_VC++ > Results_VC++.txt

This execute the program and the store the results in a file Results_VC++.txt

After this you can delete 
	file_generator.exe
	benchmark_VC++.exe
	input.bin
	
	
