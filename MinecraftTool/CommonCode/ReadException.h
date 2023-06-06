#pragma once
#include <string>

class ReadException final
{
public:
	ReadException(const std::string& what)
		:m_What{ what } {}

	~ReadException() = default;

	std::string What() const
	{
		return "Read Exception: " + m_What + '\n';
	}

private:
	std::string m_What{};
};