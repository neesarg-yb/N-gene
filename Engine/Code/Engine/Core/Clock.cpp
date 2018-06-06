#pragma once
#include "Clock.hpp"

#include <windows.h>
#define WIN32_LEAN_AND_MEAN

Clock::Clock( Clock* parent /* = nullptr */ )
	: m_parent( parent )
{
	InitalizeSecondsPerClockCycle();

	m_startHPC		= GetPerformanceCounter();
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

void Clock::Reset()
{
	m_lastFrameHPC = GetPerformanceCounter();

	frame = TimeUnits( 0 );
	total = TimeUnits( 0 );
}

void Clock::BeginFrame()
{
	uint64_t currentHPC = GetPerformanceCounter();
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
	frame.seconds	= GetSecondsFromPerformanceCounter( relativeElapsed );
	frame.ms		= GetMillliSecondsFromPerformanceCounter( relativeElapsed );

	total.hpc		+= frame.hpc;
	total.seconds	+= frame.seconds;
	total.ms		+= frame.ms;

	for each (Clock* childClock in m_childrenClocks)
	{
		childClock->AdvanceClock( relativeElapsed );
	}
}

double Clock::GetFrameDeltaSeconds()
{
	return frame.seconds;
}

void Clock::InitalizeSecondsPerClockCycle()
{
	LARGE_INTEGER frq;
	QueryPerformanceFrequency( &frq );

	m_frequency =  *( (uint64_t*) &frq );
	m_secondsPerClockCycle = 1.0 / m_frequency;
}

uint64_t Clock::GetPerformanceCounter()
{
	LARGE_INTEGER hpc;
	QueryPerformanceCounter( &hpc );

	return *( (uint64_t*) &hpc );
}

double Clock::GetSecondsFromPerformanceCounter( uint64_t hpc )
{
	return (double)hpc * m_secondsPerClockCycle;
}

unsigned int Clock::GetMillliSecondsFromPerformanceCounter( uint64_t hpc )
{
	return (unsigned int)( (double)hpc * m_secondsPerClockCycle * 1000.0 );
}

void Clock::AddChild( Clock* childClock )
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