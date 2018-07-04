#pragma once
#include <queue>
#include <vector>
#include "Engine/Renderer/External/glcorearb.h"
#include "Engine/Profiler/ProfileLogScoped.hpp"

#define MAX_HISTORY_COUNT 256

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
	void AddChild( ProfileMeasurement *child )
	{
		children.push_back( child );
	}
	void StartMeasurement()
	{
		startHPC = ProfileLogScoped::GetPerformanceCounter();
	}
	void FinishMeasurement()
	{
		endHPC = ProfileLogScoped::GetPerformanceCounter();
	}
};

class Profiler
{
public:
	 Profiler();
	~Profiler();

public:
	static Profiler*	s_instance;
	static Profiler*	GetInstace();

private:
	// Pause and Resume
	bool				m_paused	= false;
	bool				m_isPausing	= false;

private:
	// Measurement History
	ProfileMeasurement	*m_activeNode;
	ProfileMeasurement*	 m_measurementHistory[ MAX_HISTORY_COUNT ];

	// Managing the History
	int					m_currentReportIndex	=	-1;
	void				AddReportToHistoryArray( ProfileMeasurement* newReport );

public:
	void				Push( std::string const &id );
	void				Pop();
	void				MarkFrame();

public:
	void				Pause();
	void				Resume();
	
public:
	ProfileMeasurement* CreateMeasurement( std::string const &id );
	void				DestroyMeasurementTreeRecursively( ProfileMeasurement* root );
};