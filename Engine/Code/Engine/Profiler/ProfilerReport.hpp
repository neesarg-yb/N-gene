#pragma once
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfileReportEntry.hpp"

class ProfileReport
{
public:
	 ProfileReport();
	~ProfileReport();

public:
	ProfileReportEntry *m_root	= nullptr;

public:
	void	GenerateReportFromFrame( ProfileMeasurement *root );
	void	SortBySelfTime();
	void	SortByTotalTime();
	double	GetTotalFrameTime();

public:
	void	PrintToDevConsole();
};