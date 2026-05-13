#include "RPN.hpp"

#include <sstream>
#include <stdexcept>

RPN::RPN()
{
}

RPN::RPN(const RPN& other)
{
	(void)other;
}

RPN& RPN::operator=(const RPN& other)
{
	(void)other;
	return *this;
}

RPN::~RPN()
{
}

bool RPN::isOperator(const std::string& token)
{
	return (token.length() == 1
		&& (token[0] == '+' || token[0] == '-'
			|| token[0] == '*' || token[0] == '/'));
}

bool RPN::isNumberToken(const std::string& token)
{
	return (token.length() == 1 && token[0] >= '0' && token[0] <= '9');
}

long RPN::applyOperator(long lhs, long rhs, const std::string& op)
{
	if (op == "+")
		return lhs + rhs;
	if (op == "-")
		return lhs - rhs;
	if (op == "*")
		return lhs * rhs;
	if (rhs == 0)
		throw std::runtime_error("Error");
	return lhs / rhs;
}

long RPN::evaluate(const std::string& expression) const
{
	std::istringstream iss(expression);
	std::stack<long> values;
	std::string token;

	if (expression.empty())
		throw std::runtime_error("Error");
	while (iss >> token)
	{
		if (isNumberToken(token))
			values.push(token[0] - '0');
		else if (isOperator(token))
		{
			long rhs;
			long lhs;

			if (values.size() < 2)
				throw std::runtime_error("Error");
			rhs = values.top();
			values.pop();
			lhs = values.top();
			values.pop();
			values.push(applyOperator(lhs, rhs, token));
		}
		else
			throw std::runtime_error("Error");
	}
	if (values.size() != 1)
		throw std::runtime_error("Error");
	return values.top();
}
