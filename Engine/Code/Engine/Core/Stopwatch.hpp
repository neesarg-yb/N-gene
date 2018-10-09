#pragma once
#include "Engine/Core/Clock.hpp"

class Stopwatch
{
public:
	 Stopwatch( Clock const *referenceClock = nullptr );	// Defaults to master clock if no reference clock is provided!
	~Stopwatch();

private:
	uint64_t	 m_referenceTotalHPC	= 0U;				// Total HPC of the Reference Clock, when the clock got assigned to the Stopwatch
	uint64_t	 m_intervalHPC			= 0U;
	Clock const	*m_referenceClock		= nullptr;

public:
	// Changes the reference clock
	// Should maintain the elapsed time.
	// If passed a nullptr, sets the clock to the Master Clock
	void SetClock( Clock const *refClock );

	// Sets elapsedHPC to ZERO &
	// Sets intervalHPC to seconds given in argument
	void SetTimer( double seconds );

	// Sets referenceTotalHPC to current total HPC
	// Thus Elapsed Time will be zero, after this call
	void Reset();

	// Checks if the interval has elapsed, it will return true & does a reset
	// if not, it will just return false (no reset)
	bool CheckAndReset();

	// Returns true if the interval has elapsed, 
	// if so the interval gets decremented from total elapsed time
	bool Decrement();

	// Works like Decrement(), but returns how many times we have elapsed our timer,
	// and decrements all of that time in one shot
	uint DecrementAll();

	double GetIntervalSeconds() const;

	// If we really wanna use it as timer,
	// this returns how many SECONDS since the last reset
	inline double	GetElapsedTime() const	{ return Clock::GetSecondsFromHPC( GetElapsedHPC() ); }
	inline uint64_t	GetElapsedHPC() const	{ return (m_referenceClock->total.hpc - m_referenceTotalHPC); }

	// Elapsed time divided by interval
	float GetNormalizedElapsedTime() const;

	// Returns true if interval has elapsed
	bool HasElapsed() const;
};
