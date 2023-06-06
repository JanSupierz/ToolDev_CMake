#pragma once
#include <string>

class ReadException final
{
public:
	ReadException(const std::string& what);

	~ReadException() = default;

	std::string What() const;

private:
	std::string m_What{};
};