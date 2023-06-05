#pragma once
#include <string>

class ReadException final
{
public:
	ReadException(const std::string& what);
	~ReadException() = default;

	void What() const;
private:
	std::string m_What{};
};