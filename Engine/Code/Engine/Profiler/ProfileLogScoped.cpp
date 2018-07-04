#pragma once
#include "ProfileLogScoped.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Profiler/Profiler.hpp"

ProfileLogScoped::ProfileLogScoped( char const *tag )
	: m_tag( tag )
{
	m_hpcStart = Profiler::GetPerformanceCounter();
}

ProfileLogScoped::~ProfileLogScoped()
{
	uint64_t	hpcEnd			= Profiler::GetPerformanceCounter();
	uint64_t	hpcDifference	= hpcEnd - m_hpcStart;
	double		seconds			= Profiler::GetSecondsFromPerformanceCounter( hpcDifference );
	double		milliseconds	= Profiler::GetMillliSecondsFromPerformanceCounter( hpcDifference );

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

