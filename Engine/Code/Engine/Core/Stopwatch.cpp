#pragma once
#include "Stopwatch.hpp"


Stopwatch::Stopwatch( Clock const *referenceClock /*= nullptr */ )
	: m_referenceClock( referenceClock )
{
	// Defaults to the Master Clock
	if( m_referenceClock == nullptr )
		m_referenceClock = GetMasterClock();

	// Sets the reference HPC
	m_referenceTotalHPC = m_referenceClock->total.hpc;
}

Stopwatch::~Stopwatch()
{
	// I don't own the Reference Clock
	m_referenceClock = nullptr;
}

void Stopwatch::SetClock( Clock const *refClock )
{
	// Store it to maintain (even after the clock change)
	uint64_t elapsedHPC = GetElapsedHPC();

	// Change the clock
	if( refClock != nullptr )
		m_referenceClock = refClock;
	else
		m_referenceClock = GetMasterClock();

	m_referenceTotalHPC  = m_referenceClock->total.hpc;		// Sets the reference HPC to new clock's total
	m_referenceTotalHPC -= elapsedHPC;						// We maintain the elapsed time
}

void Stopwatch::SetTimer( double seconds )
{
	// Makes the elapsed time ZERO
	Reset();

	// Set the new interval
	m_intervalHPC = Clock::GetHPCFromSeconds( seconds );
}

void Stopwatch::Reset()
{
	// Set reference HPC to current total; i.e. starting the timer from this moment
	// i.e. Elapsed Time will be ZERO
	m_referenceTotalHPC = m_referenceClock->total.hpc;
}

bool Stopwatch::CheckAndReset()
{
	uint64_t elapsedHPC = GetElapsedHPC();

	if( elapsedHPC >= m_intervalHPC )
	{
		Reset();											// i.e. Elapsed Time is now reset to ZERO
		return true;
	}
	else
		return false;
}

bool Stopwatch::Decrement()
{
	if( m_intervalHPC == 0U )
		return false;

	uint64_t elapsedHPC = GetElapsedHPC();

	if( elapsedHPC >= m_intervalHPC )
	{
		m_referenceTotalHPC += m_intervalHPC;				// Adding to referenceTotalHPC => Decrementing the elapsed time
		return true;
	}
	else
		return false;
}

uint Stopwatch::DecrementAll()
{
	if( m_intervalHPC == 0U )
		return 0U;

	uint64_t elapsedHPC		 = GetElapsedHPC();
	uint	 numTimesElapsed = (uint)(elapsedHPC / m_intervalHPC);

	// Decrement elapsed time, all at once!
	m_referenceTotalHPC += ( numTimesElapsed * m_intervalHPC );

	return numTimesElapsed;
}

double Stopwatch::GetIntervalSeconds() const
{
	return Clock::GetSecondsFromHPC( m_intervalHPC );
}

float Stopwatch::GetNormalizedElapsedTime() const
{
	// Safety Check
	if( m_intervalHPC == 0 )
		return 1.f;

	uint64_t elapsedHPC				= GetElapsedHPC();
	float	 normalizedElapsedHPC	= (float)((double)elapsedHPC / (double)m_intervalHPC);

	return normalizedElapsedHPC;
}

bool Stopwatch::HasElapsed() const
{
	uint64_t elapsedHPC = GetElapsedHPC();

	if( elapsedHPC >= m_intervalHPC )
		return true;
	else
		return false;
}
