#include "BitcoinExchange.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

BitcoinExchange::BitcoinExchange(void)
{
}

BitcoinExchange::BitcoinExchange(const std::string& dbPath)
{
	this->loadDatabase(dbPath);
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) : _rates(other._rates)
{
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other)
{
	if (this != &other)
		this->_rates = other._rates;
	return *this;
}

BitcoinExchange::~BitcoinExchange(void)
{
}

std::string BitcoinExchange::trim(const std::string& s)
{
	std::string::size_type start = 0;
	std::string::size_type end = s.length();

	while (start < end && (s[start] == ' ' || s[start] == '\t'
			|| s[start] == '\n' || s[start] == '\r'
			|| s[start] == '\f' || s[start] == '\v'))
		++start;
	while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t'
			|| s[end - 1] == '\n' || s[end - 1] == '\r'
			|| s[end - 1] == '\f' || s[end - 1] == '\v'))
		--end;
	return s.substr(start, end - start);
}

bool BitcoinExchange::isLeapYear(int year)
{
	if (year % 400 == 0)
		return true;
	if (year % 100 == 0)
		return false;
	return (year % 4 == 0);
}

int BitcoinExchange::daysInMonth(int year, int month)
{
	static const int days[12] = {
		31, 28, 31, 30, 31, 30,
		31, 31, 30, 31, 30, 31
	};

	if (month == 2 && isLeapYear(year))
		return 29;
	return days[month - 1];
}

bool BitcoinExchange::isValidDate(const std::string& date)
{
	int year;
	int month;
	int day;
	char dash1;
	char dash2;
	std::istringstream iss(date);

	if (date.length() != 10 || date[4] != '-' || date[7] != '-')
		return false;
	for (std::string::size_type i = 0; i < date.length(); ++i)
	{
		if (i == 4 || i == 7)
			continue;
		if (date[i] < '0' || date[i] > '9')
			return false;
	}
	if (!(iss >> year >> dash1 >> month >> dash2 >> day))
		return false;
	if (!iss.eof() || dash1 != '-' || dash2 != '-')
		return false;
	if (month < 1 || month > 12)
		return false;
	if (day < 1 || day > daysInMonth(year, month))
		return false;
	return true;
}

bool BitcoinExchange::parsePositiveNumber(const std::string& s, double& out)
{
	std::istringstream iss(trim(s));
	char extra;

	if (!(iss >> out))
		return false;
	if (iss >> extra)
		return false;
	return true;
}

std::string BitcoinExchange::normalizeDatabaseDate(const std::string& rawDate)
{
	std::string trimmed = trim(rawDate);
	std::string::size_type firstSlash;
	std::string::size_type secondSlash;
	std::string yearPart;
	std::string monthPart;
	std::string dayPart;
	long year;
	long month;
	long day;
	std::ostringstream oss;

	if (trimmed.find('-') != std::string::npos)
		return trimmed;
	firstSlash = trimmed.find('/');
	if (firstSlash == std::string::npos)
		return trimmed;
	secondSlash = trimmed.find('/', firstSlash + 1);
	if (secondSlash == std::string::npos)
		return trimmed;
	yearPart = trimmed.substr(0, firstSlash);
	monthPart = trimmed.substr(firstSlash + 1, secondSlash - firstSlash - 1);
	dayPart = trimmed.substr(secondSlash + 1);
	if (yearPart.empty() || monthPart.empty() || dayPart.empty())
		return trimmed;
	year = std::strtol(yearPart.c_str(), 0, 10);
	month = std::strtol(monthPart.c_str(), 0, 10);
	day = std::strtol(dayPart.c_str(), 0, 10);
	oss << year << '-';
	if (month < 10)
		oss << '0';
	oss << month << '-';
	if (day < 10)
		oss << '0';
	oss << day;
	return oss.str();
}

bool BitcoinExchange::parseDatabaseLine(const std::string& line, std::string& date, double& rate)
{
	std::string trimmed = trim(line);
	std::string::size_type separator;
	std::string rawDate;
	std::string rawRate;

	if (trimmed.empty())
		return false;
	separator = trimmed.find(',');
	if (separator == std::string::npos)
		separator = trimmed.find('\t');
	if (separator == std::string::npos)
		return false;
	rawDate = trimmed.substr(0, separator);
	rawRate = trimmed.substr(separator + 1);
	date = normalizeDatabaseDate(rawDate);
	if (!isValidDate(date))
		return false;
	if (!parsePositiveNumber(rawRate, rate))
		return false;
	return true;
}

bool BitcoinExchange::parseInputLine(const std::string& line, std::string& date, double& value)
{
	std::string::size_type separator = line.find('|');
	std::string rawValue;

	if (separator == std::string::npos)
		return false;
	date = trim(line.substr(0, separator));
	rawValue = trim(line.substr(separator + 1));
	if (!isValidDate(date))
		return false;
	if (!parsePositiveNumber(rawValue, value))
		return false;
	return true;
}

bool BitcoinExchange::findRateForDate(const std::string& date, double& rate) const
{
	std::map<std::string, double>::const_iterator it = this->_rates.upper_bound(date);

	if (it == this->_rates.begin())
		return false;
	--it;
	rate = it->second;
	return true;
}

void BitcoinExchange::loadDatabase(const std::string& dbPath)
{
	std::ifstream file(dbPath.c_str());
	std::string line;

	if (!file.is_open())
		throw std::runtime_error("Error: could not open file.");
	if (!std::getline(file, line))
		throw std::runtime_error("Error: could not open file.");
	while (std::getline(file, line))
	{
		std::string date;
		double rate;

		if (trim(line).empty())
			continue;
		if (parseDatabaseLine(line, date, rate))
			this->_rates[date] = rate;
	}
	if (this->_rates.empty())
		throw std::runtime_error("Error: could not open file.");
}

void BitcoinExchange::processInputFile(const std::string& inputPath) const
{
	std::ifstream file(inputPath.c_str());
	std::string line;

	if (!file.is_open())
		throw std::runtime_error("Error: could not open file.");
	if (!std::getline(file, line))
		return;
	while (std::getline(file, line))
	{
		std::string trimmedLine = trim(line);
		std::string date;
		std::string valueField;
		std::string::size_type separator;
		double value;
		double rate;

		if (trimmedLine.empty())
			continue;
		separator = line.find('|');
		if (separator == std::string::npos)
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue;
		}
		date = trim(line.substr(0, separator));
		valueField = trim(line.substr(separator + 1));
		if (!isValidDate(date))
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue;
		}
		if (!valueField.empty() && valueField[0] == '-')
		{
			std::cout << "Error: not a positive number." << std::endl;
			continue;
		}
		if (!parsePositiveNumber(valueField, value))
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue;
		}
		if (value > 1000.0)
		{
			std::cout << "Error: too large a number." << std::endl;
			continue;
		}
		if (!findRateForDate(date, rate))
		{
			std::cout << "Error: bad input => " << line << std::endl;
			continue;
		}
		std::cout << date << " => " << value << " = " << (value * rate) << std::endl;
	}
}
