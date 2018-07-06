#pragma once
#include <algorithm>
#include "ProfilerReport.hpp"
#include "Engine/Core/DevConsole.hpp"

bool PR_AHaveHigherSelfTime( ProfileReportEntry* a, ProfileReportEntry* b)
{
	return (a->m_selfHPC) > (b->m_selfHPC);
}

bool PR_AHaveHigherTotalTime( ProfileReportEntry* a, ProfileReportEntry* b)
{
	return (a->m_totalHPC) > (b->m_totalHPC);
}

ProfileReport::ProfileReport()
{

}

ProfileReport::~ProfileReport()
{
	if( m_root != nullptr )
		delete m_root;
}

void ProfileReport::GenerateReportFromFrame( ProfileMeasurement *root, eProfileReportType reportType /* = PROFILE_REPORT_TREE */ )
{
	m_root = new ProfileReportEntry( root->id );

	if( reportType == PROFILE_REPORT_FLAT )
		m_root->PopulateFlat( root );
	else
		m_root->PopulateTree( root );
}

void ProfileReport::SortTree( eProfileReportSort sortType /*= PROFILE_REPORT_SORT_SELF_TIME */ )
{
	if( sortType == PROFILE_REPORT_SORT_TOTAL_TIME )
		SortByTotalTime();
	else
		SortBySelfTime();
}

void ProfileReport::SortBySelfTime()
{
	ProfileReportEntryList &childern = m_root->m_children;

	std::sort( childern.begin(), childern.end(), PR_AHaveHigherSelfTime );
}

void ProfileReport::SortByTotalTime()
{
	ProfileReportEntryList &childern = m_root->m_children;

	std::sort( childern.begin(), childern.end(), PR_AHaveHigherTotalTime );
}

void ProfileReport::PrintToDevConsole()
{
	if( m_root == nullptr )
		return;

	std::vector< std::string > reportStrings;
	m_root->GetProfileReportAsStringsVector( reportStrings, 0 );

	// Print strings on DevConsole
	for( uint i = 0; i < reportStrings.size(); i++ )
		ConsolePrintf( "%s", reportStrings[i].c_str() );
}