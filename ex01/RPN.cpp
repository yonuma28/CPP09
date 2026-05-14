#include "RPN.hpp"

#include <climits>
#include <sstream>
#include <stdexcept>

namespace
{
	long checkedAdd(long lhs, long rhs)
	{
		if ((rhs > 0 && lhs > LONG_MAX - rhs)
			|| (rhs < 0 && lhs < LONG_MIN - rhs))
			throw std::runtime_error("Error");
		return lhs + rhs;
	}

	long checkedSubtract(long lhs, long rhs)
	{
		if ((rhs > 0 && lhs < LONG_MIN + rhs)
			|| (rhs < 0 && lhs > LONG_MAX + rhs))
			throw std::runtime_error("Error");
		return lhs - rhs;
	}

	long checkedMultiply(long lhs, long rhs)
	{
		if (lhs == 0 || rhs == 0)
			return 0;
		if (lhs > 0)
		{
			if (rhs > 0)
			{
				if (lhs > LONG_MAX / rhs)
					throw std::runtime_error("Error");
			}
			else if (rhs < LONG_MIN / lhs)
				throw std::runtime_error("Error");
		}
		else
		{
			if (rhs > 0)
			{
				if (lhs < LONG_MIN / rhs)
					throw std::runtime_error("Error");
			}
			else
			{
				if (lhs != 0 && rhs < LONG_MAX / lhs)
					throw std::runtime_error("Error");
			}
		}
		return lhs * rhs;
	}

	long checkedDivide(long lhs, long rhs)
	{
		if (rhs == 0)
			throw std::runtime_error("Error");
		if (lhs == LONG_MIN && rhs == -1)
			throw std::runtime_error("Error");
		return lhs / rhs;
	}
}

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
		return checkedAdd(lhs, rhs);
	if (op == "-")
		return checkedSubtract(lhs, rhs);
	if (op == "*")
		return checkedMultiply(lhs, rhs);
	return checkedDivide(lhs, rhs);
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
