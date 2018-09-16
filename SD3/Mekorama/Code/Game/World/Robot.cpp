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
	, m_targetPosition( posInTower )
	, m_nextStepPosition( posInTower )
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
	SetPositionInTower( posInTower );

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

	Vector3 targetPosVec3 = Vector3( m_targetPosition );
	if( m_stopPathFinding != true && targetPosVec3 != m_transform.GetPosition() )
	{
		// If not in middle of moving to next block
		Vector3 nextBlockPosVec3 = Vector3( m_nextStepPosition );
		if( nextBlockPosVec3 != m_transform.GetPosition() )
		{
			MoveTowardsPosition( nextBlockPosVec3, deltaSeconds );
			return;
		}

		// To get next block's position to move towards
		// Get HeatMap
		HeatMap3D* heatMap = m_parentTower->GetNewHeatMapForTargetPosition( m_targetPosition );																		// TALK ABOUT IT IN PRESENTATION 

		// Get next block position to move at
		std::vector< IntVector3 > neighbourBlocksPos = m_parentTower->GetNeighbourBlocksPos( GetPositionInTower() );

		IntVector3 currentTargetPos = GetPositionInTower();
		for each (IntVector3 neighbourPos in neighbourBlocksPos)
		{
			// If HeatValue is less than currentHeat
			float currentMinimumHeat = heatMap->GetHeat( currentTargetPos );
			float neighbourBlockHeat = heatMap->GetHeat( neighbourPos );

			// Change the target position
			if( neighbourBlockHeat < currentMinimumHeat )
				currentTargetPos = neighbourPos;
		}

		m_nextStepPosition = currentTargetPos;

		delete heatMap;
	}
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
}

void Robot::StopPathFinding()
{
	m_stopPathFinding = true;
}


void Robot::ResetPathFinding()
{
	m_stopPathFinding	= false;
	m_nextStepPosition	= IntVector3( m_transform.GetPosition() + Vector3( 0.5f, 0.5f, 0.5f ) );	// set current position as next step position
	m_targetPosition	= m_nextStepPosition;
}

void Robot::SetTargetBlock( Block &targetBlock )
{
	m_targetPosition	= targetBlock.GetMyPositionInTower();
}

IntVector3 Robot::GetPositionInTower() const
{
	return IntVector3( m_transform.GetPosition() );
}
void Robot::SetPositionInTower( IntVector3 const &posInTower )
{
	m_transform.SetPosition( posInTower );

	m_targetPosition	= posInTower;
	m_nextStepPosition	= posInTower;
}

void Robot::MoveTowardsPosition( Vector3 const &destination, float deltaSeconds )
{
	Vector3 towardsDest		= destination - m_transform.GetPosition();
	float	distance		= towardsDest.GetLength();
	float	nextStepLength	= deltaSeconds * m_speed;
	
	if( distance <= nextStepLength )					// If distance is smaller than next step, clip to destination
		m_transform.SetPosition( destination );
	else												// Else do the normal walk
	{
		towardsDest			= towardsDest.GetNormalized();
		Vector3 newPosition = m_transform.GetPosition();	
		newPosition			= newPosition + ( towardsDest * nextStepLength );

		m_transform.SetPosition( newPosition );
	}
}
