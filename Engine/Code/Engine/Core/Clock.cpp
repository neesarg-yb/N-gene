#pragma once
#include "Clock.hpp"
#include "Engine/Core/Time.hpp"

#include <windows.h>
#define WIN32_LEAN_AND_MEAN

uint64_t	Clock::s_frequency				= 0U;
double		Clock::s_secondsPerClockCycle	= 0;

Clock::Clock( Clock const * parent /* = nullptr */ )
	: m_parent( parent )
{
	CheckInitalizeSecondsPerClockCycle();

	m_startHPC		= GetCurrentHPC();
	m_lastFrameHPC	= m_startHPC;
	m_timeScale		= 1.0f;
	m_frameCount	= 0;
	m_isPaused		= false;

	if( m_parent != nullptr )
		m_parent->AddChild( this );
}

Clock::~Clock()
{

}

bool Clock::IsPaused() const
{
	return m_isPaused;
}

void Clock::Resume()
{
	m_isPaused = false;
}

void Clock::Pause()
{
	m_isPaused = true;
}

void Clock::Reset()
{
	m_lastFrameHPC = GetCurrentHPC();

	frame = TimeUnits( 0 );
	total = TimeUnits( 0 );
}

void Clock::BeginFrame()
{
	uint64_t currentHPC = GetCurrentHPC();
	uint64_t elapsedHPC = currentHPC - m_lastFrameHPC;

	AdvanceClock( elapsedHPC );

	m_lastFrameHPC		= currentHPC;
}

void Clock::AdvanceClock( uint64_t const hpcElapsed )
{
	m_frameCount++;

	uint64_t relativeElapsed = hpcElapsed;

	if( m_isPaused )
		relativeElapsed = 0;
	else 
		relativeElapsed = (uint64_t)( (double)hpcElapsed * m_timeScale );

	frame.hpc		= relativeElapsed;
	frame.seconds	= GetSecondsFromHPC( relativeElapsed );
	frame.ms		= GetMilliSecondsFromHPC( relativeElapsed );

	total.hpc		+= frame.hpc;
	total.seconds	+= frame.seconds;
	total.ms		+= frame.ms;

	for each (Clock* childClock in m_childrenClocks)
	{
		childClock->AdvanceClock( relativeElapsed );
	}
}

double Clock::GetFrameDeltaSeconds() const
{
	return frame.seconds;
}

std::string Clock::GetTimestampFromHPC( uint64_t hpc )
{
	return GetTimeAsString( (time_t)hpc );
}

double Clock::GetSecondsFromHPC( uint64_t hpc )
{
	CheckInitalizeSecondsPerClockCycle();

	return (double)hpc * s_secondsPerClockCycle;
}

unsigned int Clock::GetMilliSecondsFromHPC( uint64_t hpc )
{
	CheckInitalizeSecondsPerClockCycle();

	return (unsigned int)( (double)hpc * s_secondsPerClockCycle * 1000.0 );
}

uint64_t Clock::GetHPCFromSeconds( double seconds )
{
	CheckInitalizeSecondsPerClockCycle();

	return (uint64_t)(s_frequency * seconds);
}

uint64_t Clock::GetHPCFromMilliSeconds( unsigned int ms )
{
	CheckInitalizeSecondsPerClockCycle();

	return (uint64_t)(s_frequency * (ms/1000));
}

uint64_t Clock::GetCurrentHPC()
{
	LARGE_INTEGER hpc;
	QueryPerformanceCounter( &hpc );

	return *( (uint64_t*) &hpc );
}

std::string Clock::GetCurrentTimeStamp()
{
	return GetTimestampFromHPC( GetCurrentHPC() );
}

void Clock::AddChild( Clock* childClock ) const
{
	bool thisChildAlreadyExist = false;

	for each (Clock* existingChildClock in m_childrenClocks)
	{
		if( existingChildClock == childClock )
			thisChildAlreadyExist = true;
	}

	if( !thisChildAlreadyExist )
		m_childrenClocks.push_back( childClock );
}

void Clock::CheckInitalizeSecondsPerClockCycle()
{
	// If already initialized, return
	if( s_secondsPerClockCycle != 0 )
		return;

	// Sets the Frequency & Seconds per Clock Cycle
	LARGE_INTEGER frq;
	QueryPerformanceFrequency( &frq );

	s_frequency =  *( (uint64_t*) &frq );
	s_secondsPerClockCycle = 1.0 / s_frequency;
}
