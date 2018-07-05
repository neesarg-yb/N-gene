#pragma once
#include "ProfilerReport.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"

ProfileReport::ProfileReport()
{

}

ProfileReport::~ProfileReport()
{
	if( m_root != nullptr )
		delete m_root;
}

void ProfileReport::GenerateReportFromFrame( ProfileMeasurement *root )
{
	m_root = new ProfileReportEntry( root->id );
	m_root->PopulateTree( root );
}

void ProfileReport::PrintToDevConsole()
{
	if( m_root == nullptr )
		return;

	PrintMyDataToConsoleRecursively( m_root );
}

void ProfileReport::PrintMyDataToConsoleRecursively( ProfileReportEntry *root )
{
/*
uint		m_callCount			= 0;
uint64_t	m_totalHPC			= 0;
uint64_t	m_selfHPC			= 0;
double		m_percentTotalTime	= 0;
double		m_percentSelfTime	= 0;
*/

	std::string idStr			= Stringf( "%s: %-*s", "ID", 23, root->m_id.c_str() );
	std::string callStr			= Stringf( "%s: %-*u", "cCount", 3, root->m_callCount );
	std::string totalTimeStr	= Stringf( "%s: %-*f", "TotalMS", 10, Profiler::GetMillliSecondsFromPerformanceCounter( root->m_totalHPC ) );
	std::string selfTimeStr		= Stringf( "%s: %-*f", "SelftMS", 10, Profiler::GetMillliSecondsFromPerformanceCounter( root->m_selfHPC ) );
	std::string percentTotalStr = Stringf( "%s: %-*f", "%Total", 10, root->m_percentTotalTime );
	std::string percentSelftStr = Stringf( "%s: %-*f", "%Self", 10, root->m_percentSelfTime );

	std::string combinedStr = Stringf( "%s %s %s %s  %s  %s", idStr.c_str(), callStr.c_str(), totalTimeStr.c_str(), percentTotalStr.c_str(), selfTimeStr.c_str(), percentSelftStr.c_str() );
	ConsolePrintf( "%s", combinedStr.c_str() );

	for( ProfileReportEntryMap::iterator	childIt  = root->m_children.begin();
											childIt != root->m_children.end();
											childIt++ )
	{
		PrintMyDataToConsoleRecursively( childIt->second );
	}
}

