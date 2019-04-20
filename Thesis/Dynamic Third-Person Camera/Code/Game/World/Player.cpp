#pragma once
#include "Player.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/World/Terrain.hpp"
#include "Game/GameCommon.hpp"

Player::Player( Vector3 worldPosition, Terrain const &parentTerrain )
	: GameObject( worldPosition, Vector3::ZERO, Vector3::ONE_ALL )
	, m_terrain( &parentTerrain )
{
	// Movement Presets
	m_maxSpeed = PLAYER_MAX_SPEED;
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

void Player::InformAboutCameraForward( CameraState const &currentCamState, CB_Follow &followBehavior )
{
	bool reorientButtonJustPressed = g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_B ].keyJustPressed;
	if( reorientButtonJustPressed )
	{
		// If reorientation has started, retain the movement direction
		if( followBehavior.StartCameraReorientation() )
		{
			LockInputReferenceCameraState( currentCamState );
			DebugRender2DText( 2.f, Vector2( -120.f, -60.f), 15.f, RGBA_ORANGE_COLOR, RGBA_ORANGE_COLOR, "Reorientation Started [Reference CameraState LOCKED]" );
		}
	}

	if( InputReferenceCameraStateIsLocked() )
	{
		// i.e. Post-CameraReorientation
		Vector2	currentStickPosition	= g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
		Vector2	stickPositionDifference	= m_leftStickOnCameraStateLock - currentStickPosition;
		
		// Player wants to move in different direction?
		bool playerWantsToChangeMovementDirection = stickPositionDifference.GetLength() > 0.2f;
		if( playerWantsToChangeMovementDirection )
		{
			// Makes the input direction relative to current camera state
			UnlockInputReferenceCameraState( currentCamState );
			DebugRender2DText( 2.f, Vector2( -120.f, -80.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, "Player changed direction [Reference CameraState UNLOCKED]" );
		}
	}

	UpdateCameraForward( currentCamState );
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
	frictionForce = frictionForce * frictionCoefficient;					// Friction Force = (Friction Direction) * coefficient

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

void Player::UpdateCameraForward( CameraState const &currentCamState )
{
	if( m_lockReferenceCameraState == false )
		m_inputReferenceCameraState = currentCamState;

	m_cameraForward = m_inputReferenceCameraState.GetTransformMatrix().GetKColumn();
}

void Player::LockInputReferenceCameraState( CameraState const &camState )
{
	m_inputReferenceCameraState = camState;
	m_lockReferenceCameraState = true;

	m_leftStickOnCameraStateLock = g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
}

void Player::UnlockInputReferenceCameraState( CameraState const &camState )
{
	m_lockReferenceCameraState = false;
	m_inputReferenceCameraState = camState;
}