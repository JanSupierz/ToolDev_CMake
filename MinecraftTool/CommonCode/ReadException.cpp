#include "ReadException.h"
#include <iostream>

ReadException::ReadException(const std::string& what)
	:m_What{what}
{
}

void ReadException::What() const
{
	std::cout << "Read Exception: " << m_What << '\n';
}
