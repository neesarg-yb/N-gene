//-----------------------------------------------------------------------------------------------
// Time.hpp
//
#pragma once
#include <string>


//-----------------------------------------------------------------------------------------------
double		GetCurrentTimeSeconds();
time_t		GetCurrentRawTime();
std::string GetCurrentTimestamp();
std::string GetTimeAsString( time_t const &time );