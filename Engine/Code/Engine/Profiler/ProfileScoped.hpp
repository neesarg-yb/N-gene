#pragma once
#include <string>

//--------------------------------------------------------------
// It is there for PROFILE_SCOPE_FUNCTION macros in Profiler.hpp
//

class ProfileScoped
{
public:
	 ProfileScoped( std::string const &id );
	~ProfileScoped();
};