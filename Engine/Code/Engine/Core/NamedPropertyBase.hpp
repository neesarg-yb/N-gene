#pragma once
#include <string>

class NamedPropertyBase
{
public:
			 NamedPropertyBase() { }
	virtual ~NamedPropertyBase() { }

protected:
	std::string m_name = "NOT ASSIGNED";
};
