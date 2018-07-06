#pragma once
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfileReportEntry.hpp"

enum eProfileReportType
{
	PROFILE_REPORT_TREE = 0,
	PROFILE_REPORT_FLAT,
	NUM_PROFILE_REPORT_TYPES
};

enum eProfileReportSort
{
	PROFILE_REPORT_SORT_SELF_TIME = 0,
	PROFILE_REPORT_SORT_TOTAL_TIME,
	NUM_PROFILE_REPORT_SORTS
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
	void	SortTree( eProfileReportSort sortType = PROFILE_REPORT_SORT_SELF_TIME );
	void	SortBySelfTime();
	void	SortByTotalTime();

public:
	void	PrintToDevConsole();
};