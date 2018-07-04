#pragma once
#include "Profiler.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/Command.hpp"

void PauseTheProfiler( Command &command )
{
	UNUSED( command );
	Profiler::GetInstance()->Pause();
}

void ResumeTheProfiler( Command &command )
{
	UNUSED( command );
	Profiler::GetInstance()->Resume();
}

Profiler* Profiler::s_instance = nullptr;

Profiler* Profiler::GetInstance()
{
	if( s_instance == nullptr )
		s_instance = new Profiler();

	return s_instance;
}

Profiler::Profiler()
{
	m_activeNode = nullptr;
}

Profiler::~Profiler()
{
	for( int i = 0; i < MAX_HISTORY_COUNT; i++ )
	{
		if( m_measurementHistory[i] == nullptr )
			continue;

		delete m_measurementHistory[i];
		m_measurementHistory[i] = nullptr;
	}
}

void Profiler::Startup()
{
	GetInstance();

	// Command Register
	CommandRegister( "profiler_pause",	PauseTheProfiler );
	CommandRegister( "profiler_resume", ResumeTheProfiler );
}

void Profiler::Shutdown()
{
	if( s_instance != nullptr )
		delete s_instance;
}

void Profiler::Push( std::string const &id )
{
	if( m_paused )
		return;

	// Start a new measurement
	ProfileMeasurement *measure = CreateMeasurement( id );
	
	if( m_activeNode == nullptr )
	{
		m_activeNode = measure;
	}
	else
	{
		measure->parent = m_activeNode;
		m_activeNode->AddChild( measure );
		m_activeNode = measure;
	}
}

void Profiler::Pop()
{
	if( m_paused )
		return;

	// Someone called pop without push
	GUARANTEE_OR_DIE( m_activeNode != nullptr, "Profiler: There's an extra Pop() somewhere!!" );

	m_activeNode->FinishMeasurement();
	m_activeNode = m_activeNode->parent;
}

void Profiler::MarkFrame()
{
	if( m_activeNode != nullptr )
	{
		AddReportToHistoryArray( m_activeNode );
		Pop();
		
		// not null - someone forgot to pop
		GUARANTEE_OR_DIE( m_activeNode == nullptr, "MarkFrame: someone forgot to Pop!" );
	}

	if( m_isPausing )
	{
		m_paused		= true;
		m_isPausing		= false;
	}

	if( m_isResuming )
	{
		m_paused		= false;
		m_isResuming	= false;
	}

	Push("frame");
}

void Profiler::Pause()
{
	m_isPausing = true;
}

void Profiler::Resume()
{
	m_isResuming = true;
}

ProfileMeasurement* Profiler::CreateMeasurement( std::string const &id )
{
	ProfileMeasurement *pm = new ProfileMeasurement( id );
	pm->StartMeasurement();

	return pm;
}

void Profiler::DestroyMeasurementTreeRecursively( ProfileMeasurement* root )
{
	// For each child
	for each (ProfileMeasurement* child in root->children)
	{
		// If it has children
		if( child->children.size() != 0 )
			DestroyMeasurementTreeRecursively( child );		// delete each children

		// Delete this child after deleting all its child->childern
		delete child;
		child = nullptr;
	}
}


void Profiler::AddReportToHistoryArray( ProfileMeasurement* newReport )
{
	// Get next index to add report at
	m_currentReportIndex++;
	m_currentReportIndex = m_currentReportIndex % MAX_HISTORY_COUNT;

	// If report at that index isn't nullptr
	if( m_measurementHistory[ m_currentReportIndex ] != nullptr )
	{
		// delete it
		DestroyMeasurementTreeRecursively( m_measurementHistory[ m_currentReportIndex ] );
		m_measurementHistory[ m_currentReportIndex ] = nullptr;
	}

	// Now add a new entry there..
	m_measurementHistory[ m_currentReportIndex ] = newReport;
}