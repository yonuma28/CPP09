#include "PmergeMe.hpp"

#include <exception>
#include <iostream>

int main(int argc, char **argv)
{
	try
	{
		PmergeMe pmergeMe;
		pmergeMe.run(argc, argv);
	}
	catch (const std::exception&)
	{
		std::cerr << "Error" << std::endl;
		return 1;
	}
	return 0;
}
