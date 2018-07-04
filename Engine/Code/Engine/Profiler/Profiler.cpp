#pragma once
#include "Profiler.hpp"
#include "Engine/Core/EngineCommon.hpp"

Profiler* Profiler::s_instance = nullptr;

Profiler* Profiler::GetInstace()
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

}

void Profiler::Push( std::string const &id )
{
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
	// Someone called pop without push
	GUARANTEE_OR_DIE( m_activeNode != nullptr, "Profiler: There's an extra Pop() somewhere!!" );

	m_activeNode->FinishMeasurement();
	m_activeNode = m_activeNode->parent;
}

void Profiler::MarkFrame()
{
	if( m_activeNode != nullptr )
	{
		if( m_measurementHistory.size() >= 256 )
		{
			ProfileMeasurement* oldestM = m_measurementHistory.front();
			m_measurementHistory.pop();

			DestroyMeasurementTreeRecursively( oldestM );
		}

		m_measurementHistory.push( m_activeNode );
		Pop();

		// not null - someone forgot to pop
		GUARANTEE_OR_DIE( m_activeNode == nullptr, "MarkFrame: someone forgot to Pop!" );
	}

	Push("frame");
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
