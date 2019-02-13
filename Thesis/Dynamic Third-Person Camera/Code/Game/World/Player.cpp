#pragma once
#include "Player.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/World/Terrain.hpp"
#include "Game/GameCommon.hpp"

Player::Player( Vector3 worldPosition, Terrain const &parentTerrain )
	: GameObject( worldPosition, Vector3::ZERO, Vector3::ONE_ALL )
	, m_terrain( &parentTerrain )
{
	// Movement Presets
	m_maxSpeed = 7.f;
	m_defaultMaxSpeed = m_maxSpeed;
	
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
	// Movement Input & Gravity
	ApplyMovementForces();

	// Air Drag, Friction, etc..
	ApplyResistantForces();
	
	// Updates the position based on acceleration & velocity
	GameObject::Update( deltaSeconds );

	// Makes sure that I'm on terrain, not under it
	CheckAndSnapOnTerrainSurface();

	// Make the BB8 look according to its velocity direction
	float velocityLength = m_velocity.GetLength();
	if( velocityLength > 0.1f && m_isPlayerOnTerrainSurface )
	{
		Vector3 velocityDirection	= m_velocity.GetNormalized();
		Vector2 velocityDirectionXZ	= Vector2( velocityDirection.x, velocityDirection.z );
		float	yawDegrees			= -velocityDirectionXZ.GetOrientationDegrees() + 90.f;
		m_transform.SetRotation( Vector3( 0.f, yawDegrees, 0.f ) );
	}
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

void Player::InformAboutCameraForward( Vector3 const &cameraForward )
{
	m_cameraForward = cameraForward;
}

void Player::ApplyResistantForces()
{
	// Applying the gravity
	Vector3 gravityForce = Vector3( 0.f, -9.8f, 0.f ) * m_mass;		// F = (gravitational acceleration) * m
	ApplyForce( gravityForce );

	// If not on terrain, no friction!
	if( m_isPlayerOnTerrainSurface == false )
		return;

	// If not moving, no Air Drag or Friction
	float velocityLength = m_velocity.GetLength();
	if( velocityLength == 0.f )
		return;

	Vector3 frictionForce		= m_velocity.GetNormalized() * -1.f;		// Friction Direction = (Velocity Direction) * (Normal Force)
	float	frictionCoefficient	= m_friction * m_mass;
	frictionForce =  frictionForce * frictionCoefficient;					// Friction Force = (Friction Direction) * coefficient

	ApplyForce( frictionForce );
}

void Player::ApplyMovementForces()
{
	if( m_isPlayerOnTerrainSurface == false )
		return;

	// Move relative to camera forward
	Vector3 cameraForward = m_cameraForward;
	cameraForward.y = 0.f;
	if( cameraForward.GetLength() != 0.f )
		cameraForward = cameraForward.GetNormalized();
	else
		cameraForward = Vector3::FRONT;

	Vector3 cameraRight = Vector3::CrossProduct( Vector3::UP, cameraForward );

	// Get input from Xbox Controller
	XboxController &controller			= g_theInput->m_controller[0];
	Vector2			inputDirectionXZ	= controller.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
	bool			jump				= controller.m_xboxButtonStates[ XBOX_BUTTON_A ].keyJustPressed;

	// Applying input force
	Vector2 forceXZ					= inputDirectionXZ * m_xzMovementForce;
	Vector3 forceRelativeToCamera	= ( cameraForward * forceXZ.y ) + ( cameraRight * forceXZ.x );
	ApplyForce( forceRelativeToCamera );

	// Apply Jump
	if( jump == true )
	{
		float	jumpImpulseMagnitude = 3000.f;
		Vector3 jumpImpulse			 = Vector3( 0.f, 1.f, 0.f ) * jumpImpulseMagnitude;
		ApplyForce( jumpImpulse );
	}

	// Sprint - Change Max Speed
	bool isSprinting = controller.m_xboxButtonStates[ XBOX_BUTTON_B ].keyIsDown;
	if( isSprinting )
		m_maxSpeed = m_defaultMaxSpeed * 2.f;
	else
		m_maxSpeed = m_defaultMaxSpeed;
}

void Player::CheckAndSnapOnTerrainSurface()
{
	Vector3 worldPosition	= m_transform.GetWorldPosition();
	Vector2 worldXZPosition	= Vector2( worldPosition.x, worldPosition.z );
	float	snapYCoordinate	= m_terrain->GetYCoordinateForMyPositionAt( worldXZPosition, m_bodyRadius );

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
