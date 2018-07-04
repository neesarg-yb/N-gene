#pragma once
#include <queue>
#include <vector>
#include "Engine/Renderer/External/glcorearb.h"
#include "Engine/Profiler/ProfileLogScoped.hpp"

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

public:
	ProfileMeasurement					*m_activeNode;
	std::queue< ProfileMeasurement* >	 m_measurementHistory;

public:
	void Push( std::string const &id );
	void Pop();
	void MarkFrame();
	
	ProfileMeasurement* CreateMeasurement( std::string const &id );
	void				DestroyMeasurementTreeRecursively( ProfileMeasurement* root );
};