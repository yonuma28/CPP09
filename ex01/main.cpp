#include "RPN.hpp"

#include <exception>
#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Error" << std::endl;
		return 1;
	}
	try
	{
		RPN rpn;
		long result = rpn.evaluate(argv[1]);
		std::cout << result << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
	return 0;
}
