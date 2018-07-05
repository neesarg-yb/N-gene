#pragma once
#include "ProfileReportEntry.hpp"

ProfileReportEntry::ProfileReportEntry( std::string const &id )
	: m_id( id )
{

}

ProfileReportEntry::~ProfileReportEntry()
{
	for( ProfileReportEntryMap::iterator childIt  = m_children.begin();
										 childIt != m_children.end();
										 childIt++ )
	{
		delete childIt->second;
		childIt->second = nullptr;
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
	for( ProfileReportEntryMap::iterator it  = m_children.begin();
										 it != m_children.end();
										 it++ )
	{
		if( it->first == childID )
			return it->second;
	}

	ProfileReportEntry* newChild = new ProfileReportEntry( childID );
	m_children[ childID ] = newChild;

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

