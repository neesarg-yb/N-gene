#pragma once
#include <queue>
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Profiler/ProfileScoped.hpp"
#include "Engine/Profiler/ProfileLogScoped.hpp"
#include "Engine/Renderer/External/glcorearb.h"

#define MAX_HISTORY_COUNT 256

#define PROFILE_LOG_SCOPE(tag)			ProfileLogScoped __timer__ ##__LINE__ ## (tag)
#define PROFILE_LOG_SCOPE_FUNCTION()	ProfileLogScoped __timer__ ##__LINE__ ## (__FUNCTION__)

#define PROFILE_SCOPE(tag)				ProfileScoped __timer__ ##__LINE__ ## (tag)
#define PROFILE_SCOPE_FUNCTION()		ProfileScoped __timer__ ##__LINE__ ## (__FUNCTION__)

struct ProfileMeasurement;

class Profiler
{
public:
	 Profiler();
	~Profiler();

public:
	static void			Startup();
	static void			Shutdown();

public:
	static Profiler*	s_instance;
	static Profiler*	GetInstance();

private:
	// Pause and Resume
	bool				m_paused		= false;
	bool				m_isPausing		= false;
	bool				m_isResuming	= false;

public:
	// Measurement History
	ProfileMeasurement	*m_activeNode;
	ProfileMeasurement*	 m_measurementHistory[ MAX_HISTORY_COUNT ] = { nullptr };

	// Managing the History
	int					m_currentReportIndex	=	-1;
	void				AddReportToHistoryArray( ProfileMeasurement* newReport );

public:
	// Access the History
	ProfileMeasurement*	GetPreviousFrame( uint skip_count = 0 ); 

public:
	void				Push( std::string const &id );
	void				Pop();
	void				MarkFrame();
	void				Pause();
	void				Resume();
	
public:
	ProfileMeasurement* CreateMeasurement( std::string const &id );
	void				DestroyMeasurementTreeRecursively( ProfileMeasurement* root );

public:
	// Static utility functions
	static double		s_secondsPerClockCycle;

	static uint64_t		GetPerformanceCounter();
	static double		CalculateSecondsPerClockCycle();
	static double		GetSecondsFromPerformanceCounter( uint64_t hpc );
	static double		GetMillliSecondsFromPerformanceCounter( uint64_t hpc );

private:
	int					ModuloNonNegative( int operatingOn, int moduloBy );			// It is like normal modulo, but on negative values it wraps around the range => (-1) will give you (moduloBy - 1)
};

struct ProfileMeasurement
{
	// Measurement Data
	std::string	id;
	uint64_t	startHPC;
	uint64_t	endHPC;

	// Tree Data
	ProfileMeasurement					*parent		= nullptr;
	std::vector< ProfileMeasurement* >	 children;

	// Constructor
	ProfileMeasurement( std::string id )
	{
		this->id = id;
	}

	// Functions
	void		AddChild( ProfileMeasurement *child )
	{
		children.push_back( child );
	}
	void		StartMeasurement()
	{
		startHPC = Profiler::GetPerformanceCounter();
	}
	void		FinishMeasurement()
	{
		endHPC = Profiler::GetPerformanceCounter();
	}
	uint64_t	GetElapsedHPC()
	{
		return (endHPC - startHPC);
	}
};