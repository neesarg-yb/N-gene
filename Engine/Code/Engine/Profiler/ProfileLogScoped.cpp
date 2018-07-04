#pragma once
#include <windows.h>
#define  WIN32_LEAN_AND_MEAN

#include "ProfileLogScoped.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"

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

	DebuggerPrintf( Stringf("\n-----------\nProfiling \"%s\"\n----------\nMilliseconds Difference: %lf\nSeconds      Difference: %lf\n\n", m_tag, milliseconds, seconds ).c_str() );
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

