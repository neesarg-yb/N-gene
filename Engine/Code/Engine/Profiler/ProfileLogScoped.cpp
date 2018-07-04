#pragma once
#include <windows.h>
#define  WIN32_LEAN_AND_MEAN

#include "ProfileLogScoped.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"

ProfileLogScoped::ProfileLogScoped( char const *tag )
	: m_tag( tag )
{
	m_hpcStart = GetPerformanceCounter();
}

ProfileLogScoped::~ProfileLogScoped()
{
	uint64_t	hpcEnd			= GetPerformanceCounter();
	uint64_t	hpcDifference	= hpcEnd - m_hpcStart;
	double		seconds			= GetSecondsFromPerformanceCounter( hpcDifference );
	double		milliseconds	= GetMillliSecondsFromPerformanceCounter( hpcDifference );

	std::string profilingHeadStr	= Stringf("Profiling \"%s\"", m_tag );
	std::string millisecondsStr		= Stringf("Milliseconds Difference: %lf", milliseconds );
	std::string secondsStr			= Stringf("Seconds      Difference: %lf", seconds );

	// Print on Console
	ConsolePrintf( RGBA_WHITE_COLOR, " " );
	ConsolePrintf( RGBA_WHITE_COLOR, profilingHeadStr.c_str() );
	ConsolePrintf( RGBA_GRAY_COLOR,  secondsStr.c_str() );
	ConsolePrintf( RGBA_GRAY_COLOR,  millisecondsStr.c_str() );

	// Print in Debugger Output
	DebuggerPrintf( Stringf( "\n----------\n%s\n----------\n%s\n%s\n\n", profilingHeadStr.c_str(), secondsStr.c_str(), millisecondsStr.c_str() ).c_str() );
}

uint64_t ProfileLogScoped::GetPerformanceCounter() const
{
	LARGE_INTEGER hpc;
	QueryPerformanceCounter( &hpc );

	return *( (uint64_t*) &hpc );
}

double ProfileLogScoped::GetSecondsFromPerformanceCounter( uint64_t hpc ) const
{
	LARGE_INTEGER frq;
	QueryPerformanceFrequency( &frq );

	uint64_t	frequency				=  *( (uint64_t*) &frq );
	double		secondsPerClockCycle	= 1.0 / frequency;

	return (double)hpc * secondsPerClockCycle;
}

double ProfileLogScoped::GetMillliSecondsFromPerformanceCounter( uint64_t hpc ) const
{
	LARGE_INTEGER frq;
	QueryPerformanceFrequency( &frq );

	uint64_t	frequency				=  *( (uint64_t*) &frq );
	double		secondsPerClockCycle	= 1.0 / frequency;

	return ( (double)hpc * secondsPerClockCycle * 1000.0 );
}

