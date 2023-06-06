#include "ReadException.h"

ReadException::ReadException(const std::string& what)
	:m_What{ what }
{
}

std::string ReadException::What() const
{
	return "Read Exception: " + m_What + '\n';
}
