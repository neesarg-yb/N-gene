#pragma once
#include "Stopwatch.hpp"


Stopwatch::Stopwatch( Clock const *referenceClock /*= nullptr */ )
	: m_referenceClock( referenceClock )
{
	// Defaults to the Master Clock
	if( m_referenceClock == nullptr )
		m_referenceClock = GetMasterClock();

	m_startHPC = m_referenceClock->GetCurrentHPC();
}

Stopwatch::~Stopwatch()
{
	// I don't own the Reference Clock
	m_referenceClock = nullptr;
}

void Stopwatch::SetClock( Clock const *refClock )
{
	// Store it to maintain (even after the clock change)
	uint64_t elapsedTime = m_referenceClock->GetCurrentHPC() - m_startHPC;

	// Change the clock
	if( refClock != nullptr )
		m_referenceClock = refClock;
	else
		m_referenceClock = GetMasterClock();

	m_startHPC  = m_referenceClock->GetCurrentHPC();			// Set startHPC to current time
	m_startHPC -= elapsedTime;									// Maintain the elapsed time
}

void Stopwatch::SetTimer( double seconds )
{
	m_startHPC		= m_referenceClock->GetCurrentHPC();
	m_intervalHPC	= Clock::GetHPCFromSeconds( seconds );
}

void Stopwatch::Reset()
{
	m_startHPC = m_referenceClock->GetCurrentHPC();
}

bool Stopwatch::CheckAndReset()
{
	uint64_t elapsedTime = m_referenceClock->GetCurrentHPC() - m_startHPC;

	if( elapsedTime >= m_intervalHPC )
	{
		m_startHPC = m_referenceClock->GetCurrentHPC();		// i.e. Elapsed Time is now reset to ZERO
		return true;
	}
	else
		return false;
}

bool Stopwatch::Decrement()
{
	uint64_t elapsedTime = m_referenceClock->GetCurrentHPC() - m_startHPC;

	if( elapsedTime >= m_intervalHPC )
	{
		m_startHPC += m_intervalHPC;								// Adding to startHPC => Decrementing the elapsed time
		return true;
	}
	else
		return false;
}

uint Stopwatch::DecrementAll()
{
	uint64_t elapsedTime		= m_referenceClock->GetCurrentHPC() - m_startHPC;
	uint	 numTimesElapsed	= (uint)(elapsedTime % m_intervalHPC);

	// Decrement elapsed time, all at once!
	m_startHPC += ( numTimesElapsed * m_intervalHPC );

	return numTimesElapsed;
}

double Stopwatch::GetElapsedTime() const
{
	uint64_t elapsedHPC = m_referenceClock->GetCurrentHPC() - m_startHPC;

	return Clock::GetSecondsFromHPC( elapsedHPC );
}

float Stopwatch::GetNormalizedElapsedTime() const
{
	// Safety Check
	if( m_intervalHPC == 0 )
		return 1.f;

	uint64_t elapsedHPC				= m_referenceClock->GetCurrentHPC() - m_startHPC;
	float	 normalizedElapsedHPC	= (float) (elapsedHPC / m_intervalHPC);

	return normalizedElapsedHPC;
}

bool Stopwatch::HasElapsed() const
{
	uint64_t elapsedHPC = m_referenceClock->GetCurrentHPC() - m_startHPC;

	if( elapsedHPC >= m_intervalHPC )
		return true;
	else
		return false;
}
