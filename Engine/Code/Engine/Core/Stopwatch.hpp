#pragma once
#include "Engine/Core/Clock.hpp"

class Stopwatch
{
public:
	 Stopwatch( Clock const *referenceClock = nullptr );		// Defaults to master clock if no reference clock is provided!
	~Stopwatch();

private:
	uint64_t	 m_startHPC			= 0U;
	uint64_t	 m_intervalHPC		= 0U;
	Clock const	*m_referenceClock	= nullptr;

public:
	// Changes the reference clock
	// Should maintain the elapsed time.
	// If passed a nullptr, sets the clock to the Master Clock
	void SetClock( Clock const *refClock );

	// Sets startHPC to current time
	// Sets intervalHPC to seconds given in argument
	void SetTimer( double seconds );

	// Sets startHPC to current time
	// Thus Elapsed Time will be zero, after this call
	void Reset();

	// Checks if the interval has elapsed, it will return true & does a reset
	// if not, it will just return false (no reset)
	bool CheckAndReset();

	// Returns true if the interval has elapsed, 
	// if so the interval gets decremented from total time passed
	bool Decrement();

	// Works like Decrement(), but returns how many times we have elapsed our timer,
	// and decrements all of that time in one shot
	uint DecrementAll();

	double GetIntervalSeconds() const;

	// If we really wanna use it as timer,
	// this returns how many SECONDS since the last reset
	double GetElapsedTime() const;

	// Elapsed time divided by interval
	float GetNormalizedElapsedTime() const;

	// Returns true if interval has elapsed
	bool HasElapsed() const;
};
