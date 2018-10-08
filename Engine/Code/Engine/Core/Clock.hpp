#pragma once
#include <cstdint>
#include <vector>
#include "Engine/Core/EngineCommon.hpp"

struct TimeUnits
{
	unsigned int	ms		= 0;
	double			seconds	= 0;
	uint64_t		hpc		= 0;

	TimeUnits() {};
	TimeUnits( unsigned int assignAll )
	{
		hpc		= (uint64_t) assignAll;
		seconds	= (double) assignAll;
		ms		= assignAll;
	}
	TimeUnits( unsigned int ms, double seconds, uint64_t hpc )
	{
		this->ms		= ms;
		this->seconds	= seconds;
		this->hpc		= hpc;
	}
};

class Clock
{
public:
	TimeUnits frame;
	TimeUnits total;

private:
	uint64_t				m_startHPC				= 0;
	uint64_t				m_lastFrameHPC			= 0;
	static uint64_t			s_frequency;				// Clock Cycles per Second
	static double			s_secondsPerClockCycle;		// Not exactly cycle, it is kinda secondsPerCount

	double					m_timeScale				= 1;
	unsigned int			m_frameCount			= 0;
	bool					m_isPaused				= false;

	Clock const*					m_parent		= nullptr;
	std::vector< Clock* > mutable 	m_childrenClocks;

public:
	 Clock( Clock const* parent = nullptr );
	 Clock( Clock const & copy ) = delete;
	~Clock();

public:
	void	Reset();
	void	BeginFrame();
	void	AdvanceClock( uint64_t const hpcElapsed );

	uint64_t	GetCurrentHPC() const;					// Gets HPC. Uses a MSDN Function
	double		GetFrameDeltaSeconds() const;			// FrameTime is deltaSeconds
	
	void		AddChild( Clock* childClock ) const;

private:
	static void	CheckInitalizeSecondsPerClockCycle();	// If s_secondsPerClockCycle is not initialized, it initializes it

public:
	// From HPC
	static double		GetSecondsFromHPC( uint64_t hpc );
	static unsigned int	GetMilliSecondsFromHPC( uint64_t hpc );

	// To HPC
	static uint64_t		GetHPCFromSeconds( double seconds );
	static uint64_t		GetHPCFromMilliSeconds( unsigned int ms );
};
