#include <vector>
#include <iostream>

int main(int argc, char argv[])
{
	std::vector<uint64_t> V;
	V.reserve(10);
	for (uint32_t i = 0; i < 10; ++i) V.push_back(i + 1);
	std::vector<uint64_t>::iterator Alfa = V.begin() + 10;
	while (*Alfa != 1) --Alfa;
	std::cout << "tatachan\n";
	--Alfa;
	std::cout << "NO ha pasado nada ?\n";
	return 0;
}