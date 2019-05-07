#pragma once
#include <string>

class NamedPropertyBase
{
public:
			 NamedPropertyBase( std::string const &name ) : m_name( name ) { }
	virtual ~NamedPropertyBase() { }

protected:
	std::string m_name;
};
