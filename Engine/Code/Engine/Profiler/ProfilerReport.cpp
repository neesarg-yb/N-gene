#pragma once
#include "ProfilerReport.hpp"
#include "Engine/Core/DevConsole.hpp"

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