#pragma once
#include "ProfileReportEntry.hpp"
#include "Engine/Core/StringUtils.hpp"

ProfileReportEntry::ProfileReportEntry( std::string const &id )
	: m_id( id )
{

}

ProfileReportEntry::~ProfileReportEntry()
{
	for each (ProfileReportEntry* pEntry in m_children)
	{
		delete pEntry;
		pEntry = nullptr;
	}
}

void ProfileReportEntry::PopulateTree( ProfileMeasurement* root )
{
	// We can set totalHPC & callCount for this root
	CalculateInitialData( root );

	for each (ProfileMeasurement* child in root->children)
	{
		ProfileReportEntry *entry = GetOrCreateChild( child->id );
		entry->PopulateTree( child );
	}

	// Calculate selfTime & percentages
	CalculateRemainingData( root );
}

ProfileReportEntry* ProfileReportEntry::GetOrCreateChild( std::string const &childID )
{
	for each (ProfileReportEntry* pEntry in m_children)
	{
		if( pEntry->m_id == childID )
			return pEntry;
	}

	ProfileReportEntry* newChild = new ProfileReportEntry( childID );
	m_children.push_back( newChild );

	return newChild;
}

void ProfileReportEntry::CalculateInitialData( ProfileMeasurement* node )
{
	m_callCount++;
	m_totalHPC += node->GetElapsedHPC();
}

void ProfileReportEntry::CalculateRemainingData( ProfileMeasurement* node )
{
	// Self time
	m_selfHPC = m_totalHPC - ChildrensTotalHPC( node );

	// Get total time of the root (or frame)
	uint64_t totalFrameTime = GetFramesTotalHPC( node );

	// Calculate percentage
	m_percentTotalTime	= (double) ( (long double)m_totalHPC / (long double)totalFrameTime ) * 100.0;
	m_percentSelfTime	= (double) ( (long double)m_selfHPC  / (long double)totalFrameTime ) * 100.0;
}

uint64_t ProfileReportEntry::ChildrensTotalHPC( ProfileMeasurement* root )
{
	uint64_t cTotalHPC = 0;

	for each (ProfileMeasurement* child in root->children)
		cTotalHPC += child->GetElapsedHPC();

	return cTotalHPC;
}

uint64_t ProfileReportEntry::GetFramesTotalHPC( ProfileMeasurement* child )
{
	ProfileMeasurement* possibleParent = child;

	// Traverse up to the top of the tree
	while ( possibleParent->parent != nullptr )
		possibleParent = possibleParent->parent;

	return possibleParent->GetElapsedHPC();
}

void ProfileReportEntry::GetProfileReportAsStringsVector( std::vector<std::string> &outStrings, uint herairchyLevel )
{
	// If hierarchy level ZERO => We need to provide headers, too!
	if( herairchyLevel == 0 )
	{
		std::string idStr			= Stringf( "%-*s", 50, "ID" );
		std::string callStr			= Stringf( "%-*s",  6, "Calls" );
		std::string totalTimeStr	= Stringf( "%-*s", 10, "Total MS" );
		std::string selfTimeStr		= Stringf( "%-*s", 10, "Self MS" );
		std::string percentTotalStr = Stringf( "%-*s", 10, "%Total" );
		std::string percentSelftStr = Stringf( "%-*s", 10, "%Self" );

		std::string combinedStr = Stringf( "%s  %s  %s  %s  %s  %s", idStr.c_str(), callStr.c_str(), totalTimeStr.c_str(), percentTotalStr.c_str(), selfTimeStr.c_str(), percentSelftStr.c_str() );
		outStrings.push_back( " " );
		outStrings.push_back( combinedStr );
		outStrings.push_back( " " );
	}

	std::string idStr			= Stringf( "%-*s%-*s", herairchyLevel, "", (50 - herairchyLevel), m_id.c_str() );
	std::string callStr			= Stringf( "%-*u", 6, m_callCount );
	std::string totalTimeStr	= Stringf( "%-*.2f", 10, Profiler::GetMillliSecondsFromPerformanceCounter( m_totalHPC ) );
	std::string selfTimeStr		= Stringf( "%-*.2f", 10, Profiler::GetMillliSecondsFromPerformanceCounter( m_selfHPC ) );
	std::string percentTotalStr = Stringf( "%-*.2f", 10, m_percentTotalTime );
	std::string percentSelftStr = Stringf( "%-*.2f", 10, m_percentSelfTime );

	std::string combinedStr = Stringf( "%s  %s  %s  %s  %s  %s", idStr.c_str(), callStr.c_str(), totalTimeStr.c_str(), percentTotalStr.c_str(), selfTimeStr.c_str(), percentSelftStr.c_str() );
	outStrings.push_back( combinedStr );

	for each (ProfileReportEntry* pChild in m_children)
	{
		pChild->GetProfileReportAsStringsVector( outStrings, herairchyLevel + 1 );
	}
}