#pragma once
#include "ProfileScoped.hpp"
#include "Engine/Profiler/Profiler.hpp"

ProfileScoped::ProfileScoped( std::string const &id )
{
	Profiler::GetInstance()->Push( id );
}

ProfileScoped::~ProfileScoped()
{
	Profiler::GetInstance()->Pop();
}

