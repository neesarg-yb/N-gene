#pragma once
#include <map>
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Profiler/Profiler.hpp"

class ProfileReportEntry;
typedef std::vector< ProfileReportEntry* > ProfileReportEntryList;

class ProfileReportEntry
{
public:
	 ProfileReportEntry( std::string const &id );
	~ProfileReportEntry();

public:
	std::string m_id				= "ID not assigned!";
	uint		m_callCount			= 0;
	uint64_t	m_totalHPC			= 0;
	uint64_t	m_selfHPC			= 0;
	double		m_percentTotalTime	= 0;
	double		m_percentSelfTime	= 0;

public:
	ProfileReportEntry		*m_parent	= nullptr;
	ProfileReportEntryList	 m_children;

public:
	void				PopulateTree( ProfileMeasurement* root );
	ProfileReportEntry* GetOrCreateChild( std::string const &childID );
	void				CalculateInitialData( ProfileMeasurement* node );		// Sets totalHPC & callCount
	void				CalculateRemainingData( ProfileMeasurement* node );

private:
	uint64_t			ChildrensTotalHPC( ProfileMeasurement* root );
	uint64_t			GetFramesTotalHPC( ProfileMeasurement* child );
};