#pragma once
#include <string>

class ProfileScoped
{
public:
	 ProfileScoped( std::string const &id );
	~ProfileScoped();
};