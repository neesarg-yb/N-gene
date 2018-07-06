#pragma once
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfileReportEntry.hpp"

enum eProfileReportType
{
	PROFILE_REPORT_TREE = 0,
	PROFILE_REPORT_FLAT,
	NUM_PROFILE_REPORT_TYPES
};

class ProfileReport
{
public:
	 ProfileReport();
	~ProfileReport();

public:
	ProfileReportEntry *m_root	= nullptr;

public:
	void	GenerateReportFromFrame( ProfileMeasurement *root, eProfileReportType reportType = PROFILE_REPORT_TREE );
	void	SortBySelfTime();
	void	SortByTotalTime();
	double	GetTotalFrameTime();

public:
	void	PrintToDevConsole();
};