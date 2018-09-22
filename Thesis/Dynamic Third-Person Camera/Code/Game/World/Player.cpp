#pragma once
#include "Player.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/World/Terrain.hpp"

Player::Player( Vector3 worldPosition, Terrain const &parentTerrain )
	: GameObject( worldPosition, Vector3::ZERO, Vector3::ONE_ALL )
	, m_terrain( &parentTerrain )
{
	// Setup the Renderable - BB8
//	m_renderable		= new Renderable( Vector3::ZERO, Vector3::ZERO, Vector3( 0.007f, 0.007f, 0.007f ) );
	m_renderable		= new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );
/*	bool loadSuccess	= ModelLoader::LoadObjectModelFromPath( "Data\\Models\\bb8\\BB8.obj", *m_renderable );

	// If load fails
	GUARANTEE_RECOVERABLE( loadSuccess, "Failed to load the player model!" );
	if( loadSuccess == false )
	{
		m_renderable = nullptr;
		delete m_renderable;
	}
*/

	Material *newMat	= Material::CreateNewFromFile( "Data\\Materials\\Block_Metal.material" );
	Mesh	 *newMesh	= MeshBuilder::CreateSphere( m_bodyRadius, 10U, 10U );
	m_renderable->SetBaseMaterial( newMat );
	m_renderable->SetBaseMesh( newMesh );
}

Player::~Player()
{
	m_terrain = nullptr;
}

void Player::Update( float deltaSeconds )
{
	// Add Drag
	MotionDragUpdate( deltaSeconds );

	// Apply forces according to the input & gravity
	MovementUpdate( deltaSeconds );

	// Updates the position based on acceleration & velocity
	GameObject::Update( deltaSeconds );

	// Makes sure that I'm on terrain, not under it
	CheckAndSnapOnTerrainSurface();
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

void Player::MotionDragUpdate( float deltaSeonds )
{
	UNUSED( deltaSeonds );
}

void Player::MovementUpdate( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	// Get input from Xbox Controller
	XboxController &controller			= g_theInput->m_controller[0];
	Vector2			inputDirectionXZ	= controller.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
	bool			jump				= controller.m_xboxButtonStates[ XBOX_BUTTON_A ].keyJustPressed;

	// Applying input force
	float	xzMovementForce = 10.f;
	Vector2 forceXZ			= inputDirectionXZ * xzMovementForce;
	ApplyForce( forceXZ.x, 0.f, forceXZ.y );

	// Apply Jump
	if( m_isPlayerOnTerrainSurface == true && jump == true )
	{
		float	jumpImpulseMagnitude = 300.f;
		Vector3 jumpImpulse			 = Vector3( 0.f, 1.f, 0.f ) * jumpImpulseMagnitude;
		ApplyForce( jumpImpulse );
	}

	// Applying the gravity
	Vector3 gravityForce = Vector3( 0.f, -9.8f, 0.f ) * m_mass;		// F = (gravitational acceleration) * m
	ApplyForce( gravityForce );
}

void Player::CheckAndSnapOnTerrainSurface()
{
	Vector3 worldPosition	= m_transform.GetWorldPosition();
	Vector2 worldXZPosition	= Vector2( worldPosition.x, worldPosition.z );
	float	snapYCoordinate	= m_terrain->GetYCoordinateForMyPositionAt( worldXZPosition, 0.25f );

	// If I'm under the terrain, snap me at top of its surface
	if( worldPosition.y <= snapYCoordinate )
	{
		worldPosition.y = snapYCoordinate;
		m_transform.SetPosition( worldPosition );

		// You are on terrain, so now your down velocity will be zero b/c of the collision with terrain..
		//		If your velocity was towards upwards, no problem, keep the upward velocity intact.
		if( m_velocity.y <= 0.f )
			m_velocity.y = 0.f;

		m_isPlayerOnTerrainSurface = true;
	}
	else
		m_isPlayerOnTerrainSurface = false;
}

// Newton's Second Law
void Player::ApplyForce( float x, float y, float z )
{
	m_acceleration += ( Vector3( x, y, z ) / m_mass );
}
