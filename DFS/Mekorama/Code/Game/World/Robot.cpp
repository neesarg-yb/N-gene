#pragma once
#include "Robot.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Math/HeatMap3D.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Game/theGame.hpp"
#include "Game/Level.hpp"
#include "Game/World/Tower.hpp"

Robot::Robot( IntVector3 const &posInTower, Tower *parentTower )
	: GameObject( GAMEOBJECT_TYPE_ROBOT )
	, m_posInTower( posInTower )
	, m_parentTower( parentTower )
{
	// PickID
	SetPickID( GameObject::GetNewPickID() );

	// Renderable
	m_renderable		= new Renderable( Vector3::ZERO, Vector3::ZERO, Vector3( 0.007f, 0.007f, 0.007f ) );
	bool modelLoaded	= ModelLoader::LoadObjectModelFromPath( "Data\\Models\\bb8\\BB8.obj", *m_renderable );
	GUARANTEE_OR_DIE( modelLoaded, "Error: Failed loading obj model for Robot..!" );
	m_renderable->SetPickID( GetPickID() );

	// Transform Parenting
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	Vector3 worldPos = m_transform.GetWorldPosition();
}

Robot::~Robot()
{
	delete m_renderable;
}

void Robot::Update( float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	UNUSED( deltaSeconds );
	UpdateLocalTransform();
}

void Robot::ObjectSelected()
{
	Vector3 centerPos	= m_transform.GetWorldPosition();
	Vector3 bottomLeft	= centerPos - ( Vector3::ONE_ALL * 0.5f );
	Vector3 topRight	= centerPos + ( Vector3::ONE_ALL * 0.5f );
	DebugRenderWireCube( 0.f, bottomLeft, topRight, RGBA_YELLOW_COLOR, RGBA_YELLOW_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
}

void Robot::SetParentTower( Tower &parent )
{
	m_parentTower = &parent;

	// Transform
	m_transform.SetParentAs( &m_parentTower->m_transform );
	UpdateLocalTransform();
}

void Robot::MoveAtBlock( Block &targetBlock )
{
	// Get HeatMap
	HeatMap3D* heatMap = m_parentTower->GetNewHeatMapForTargetPosition( targetBlock.GetMyPositionInTower() );

	// Get next block position to move at
	std::vector< IntVector3 > neighbourBlocksPos = m_parentTower->GetNeighbourBlocksPos( m_posInTower );

	IntVector3 currentTargetPos = m_posInTower;
	for each (IntVector3 neighbourPos in neighbourBlocksPos)
	{
		// If HeatValue is less than currentHeat
		float currentMinimumHeat = heatMap->GetHeat( currentTargetPos );
		float neighbourBlockHeat = heatMap->GetHeat( neighbourPos );

		// Change the target position
		if( neighbourBlockHeat < currentMinimumHeat )
			currentTargetPos = neighbourPos;
	}

	m_posInTower = currentTargetPos;

	delete heatMap;
}

void Robot::UpdateLocalTransform()
{
	Vector3 localPosition = Vector3( m_posInTower );
	m_transform.SetPosition( localPosition );
}
