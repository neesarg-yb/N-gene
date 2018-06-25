#pragma once
#include "Robot.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Engine/Math/HeatMap3D.hpp"
#include "Game/theGame.hpp"
#include "Game/Level.hpp"
#include "Game/World/Tower.hpp"

Robot::Robot( Vector3 const &worldPosition )
{
	// PickID
	SetPickID( GameObject::GetNewPickID() );

	// Transform
	m_transform.SetPosition( worldPosition );

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
	UNUSED( deltaSeconds );
	UpdateHeatMap();
}

void Robot::ObjectSelected()
{
	Vector3 centerPos	= m_transform.GetWorldPosition();
	Vector3 bottomLeft	= centerPos - ( Vector3::ONE_ALL * 0.5f );
	Vector3 topRight	= centerPos + ( Vector3::ONE_ALL * 0.5f );
	DebugRenderWireCube( 0.f, bottomLeft, topRight, RGBA_YELLOW_COLOR, RGBA_YELLOW_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
}

void Robot::UpdateHeatMap()
{
	if( m_currentHeatMap != nullptr )
		delete m_currentHeatMap;

	Tower &currentTower = *g_theGame->m_currentLevel->m_tower;
	m_currentHeatMap = new HeatMap3D( IntVector3( 3, 2, 2 ), 99999999.f );

	m_currentHeatMap->SetHeat( 12.f, IntVector3( 2, 1, 1 ) );
	m_currentHeatMap->AddHeat( 1.0f, IntVector3( 2, 1, 1 ) );
	float heat	= m_currentHeatMap->GetHeat( IntVector3( 2, 1, 1 ) );

	m_currentHeatMap->SetHeat( 89.f, IntVector3( 0, 0, 1 ) );
	float heat2 = m_currentHeatMap->GetHeat( IntVector3( 0, 0, 1 ) );

	float jj;
}
