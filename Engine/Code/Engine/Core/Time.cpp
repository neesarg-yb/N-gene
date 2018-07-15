//-----------------------------------------------------------------------------------------------
// Time.cpp
//	

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <time.h>


//-----------------------------------------------------------------------------------------------
double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initialTime;
	static double secondsPerCount = InitializeTime( initialTime );
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}

std::string GetCurrentTimestamp()
{
	tm		tstruct;
	char	buf[80];

	time_t	now = time(0);
	localtime_s( &tstruct, &now );

	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%m-%d-%Y(%Hh %Mm %S)", &tstruct);

	return buf;
}

