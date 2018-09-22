#pragma once
#include "Player.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Game/World/Terrain.hpp"

Player::Player( Vector3 worldPosition, Terrain const &parentTerrain )
	: GameObject( worldPosition, Vector3::ZERO, Vector3::ONE_ALL )
	, m_terrain( &parentTerrain )
{
	// Setup the Renderable - BB8
	m_renderable		= new Renderable( Vector3::ZERO, Vector3::ZERO, Vector3( 0.007f, 0.007f, 0.007f ) );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );
	bool loadSuccess	= ModelLoader::LoadObjectModelFromPath( "Data\\Models\\bb8\\BB8.obj", *m_renderable );

	// If load fails
	GUARANTEE_RECOVERABLE( loadSuccess, "Failed to load the player model!" );
	if( loadSuccess == false )
	{
		m_renderable = nullptr;
		delete m_renderable;
	}
}

Player::~Player()
{
	m_terrain = nullptr;
}

void Player::Update( float deltaSeconds )
{
	// Get the velocity under max speed
	float currentSpeed = m_velocity.GetLength();
	if( currentSpeed != 0.f )
		m_velocity = m_velocity.GetNormalized() * ( currentSpeed > m_maxSpeed ? m_maxSpeed : currentSpeed );

	// Apply gravity
	m_velocity.y -= (9.8f * deltaSeconds);

	// If going below terrain, bring it on the terrain
	Vector3 worldPosition	= m_transform.GetPosition();
	float	terrainYCoord	= m_terrain->GetYCoordinateForMyPositionAt( worldPosition.x, worldPosition.z, 0.5f );

	if( worldPosition.y < terrainYCoord )
	{
		m_velocity.y = 0.f;
		worldPosition.y = terrainYCoord;

		m_transform.SetPosition( worldPosition );
	}

	// Updates the position based on velocity
	GameObject::Update( deltaSeconds );
}

void Player::AddRenderablesToScene( Scene &activeScene )
{
	// No renderable? => return
	if( m_renderable == nullptr )
		return;

	activeScene.AddRenderable( *m_renderable );
}

void Player::RemoveRenderablesFromScene( Scene &activeScene )
{
	// No renderable? => return
	if( m_renderable == nullptr )
		return;

	activeScene.RemoveRenderable( *m_renderable );
}

void Player::AddVelocity( Vector3 velocity )
{
	m_velocity += velocity;
}

void Player::AddVelocity( float x, float y, float z )
{
	AddVelocity( Vector3( x, y, z) );
}
