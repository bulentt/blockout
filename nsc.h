#pragma once

#include <string>
#include <sstream>
#include <typeinfo>

namespace nsc // number-string conversions
{

class InvalidNumberException : public std::bad_cast
{
public:
	InvalidNumberException(const std::string& invalidNumber)
		: std::bad_cast((invalidNumber + " is inavlid number !").c_str())
	{
	}
};

template <typename NumberType>
NumberType StringToNumber(const std::string& str)
{
	NumberType number;
	std::istringstream iss(str);
	
	if (iss >> number)
	{
		return number;
	}
	else
	{
		throw InvalidNumberException(str);
	}
}

template <typename NumberType>
std::string NumberToString(NumberType number)
{
	ostringstream oss;
	oss << number;
	return oss.str();
}

}
