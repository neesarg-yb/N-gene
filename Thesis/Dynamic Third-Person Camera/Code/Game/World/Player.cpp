#pragma once
#include "Player.hpp"
#include "Engine/Math/Complex.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/World/Terrain.hpp"
#include "Game/GameCommon.hpp"

Player::Player( Vector3 worldPosition, Terrain const &parentTerrain, Clock *parentClock )
	: GameObject( worldPosition, Vector3::ZERO, Vector3::ONE_ALL )
	, m_terrain( &parentTerrain )
	, m_inputInterpolationTimer( parentClock )
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
	UpdateDebugStateFromInput();

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
	if( g_theInput->WasKeyJustPressed( 'F' ) )
		ToggleInputInterpolationDisabled();

	bool reorientButtonJustPressed = g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_B ].keyJustPressed;
	if( reorientButtonJustPressed )
	{
		// If reorientation has started, retain the movement direction
		if( followBehavior.StartCameraReorientation() )
		{
			LockInputState( currentCamState );
			if( !m_inputInterpolationDisabled )
				DebugRender2DText( 2.f, Vector2( -120.f, -40.f), 15.f, RGBA_ORANGE_COLOR, RGBA_ORANGE_COLOR, "[INPUT_LOCKED] - Reorientation Started" );
		}
	}

	if( m_movementInputState == INPUT_LOCKED )
	{
		// i.e. Post-CameraReorientation
		Vector2	currentStickPosition	= g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
		Vector2	stickPositionDifference	= m_leftStickOnInputLocked - currentStickPosition;
		
		// Player wants to move in different direction?
		bool playerWantsToChangeMovementDirection = stickPositionDifference.GetLength() > m_retainInputRegionRadiusFraction;
		if( playerWantsToChangeMovementDirection )
		{
			// Makes the movement direction relative to current camera state, and then interpolate towards where player wants to go
			StartInputInterpolation( currentCamState );
			DebugRender2DText( 2.f, Vector2( -120.f, -60.f ), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, "[INPUT_INTERPOLATION] - Player initiated movement change" );

			// If Camera Reorientation is still on-going, stop it to eliminate any confusions for player
			if( m_debugAutoStopCameraReorientation )
				followBehavior.StopCameraReorientation();
		}
	}

	UpdateCameraForward( currentCamState );

	// Debug Render
	Vector2 leftStickVisPos = Vector2( 139.f, -340.f );
	DebugRenderLeftStickInput( leftStickVisPos, 250.f );
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
	
	if( m_movementInputState != INPUT_UNLOCKED )
	{
		bool playerReleasedTheLeftStick = inputDirectionXZ.GetLength() <= m_leftStickReleasedRegionRadiusFraction;
		if( playerReleasedTheLeftStick )
		{
			// Makes the movement direction relative to current camera state, and player can instantly move in the direction they want to go
			UnlockInputState();
			DebugRender2DText( 2.f, Vector2( -120.f, -80.f ), 15.f, RGBA_YELLOW_COLOR, RGBA_YELLOW_COLOR, "[INPUT_UNLOCKED] - Player stopped moving" );
		}
	}

	if( m_movementInputState == INPUT_LOCKED )
	{
		// Keep moving the same way, if we went through camera reorientation, until the lock is lifted
		inputDirectionXZ = m_leftStickOnInputLocked;
	}
	else if( m_movementInputState == INPUT_INTERPOLATION )
	{
		float t = m_inputInterpolationTimer.GetNormalizedElapsedTime();

		// Interpolation Interpolation - It is more like rotating a vector towards the other one
		m_leftStickWhenInterpolating = InterpolateInput( m_leftStickOnInputLocked, inputDirectionXZ, t );
		inputDirectionXZ = m_leftStickWhenInterpolating;

		if( t >= 1.f )
		{
			UnlockInputState();
			DebugRender2DText( 2.f, Vector2( -120.f, -80.f ), 15.f, RGBA_BLUE_COLOR , RGBA_BLUE_COLOR, "[INPUT_UNLOCKED] - Input Interpolation finished" );
		}
	}

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

Vector2 Player::InterpolateInput( Vector2 const &a, Vector2 const &b, float t ) const
{
	t = ClampFloat01( t );

	Vector2 aDir = a;
	Vector2 bDir = b;

	float aLength = aDir.NormalizeAndGetLength();
	float bLength = bDir.NormalizeAndGetLength();

	Complex interpolatedDir		= Complex( aDir.x, aDir.y ).TurnTowardByFraction( Complex( bDir.x, bDir.y ), t );
	float	interpolatedLength	= Interpolate( aLength, bLength, t );

	return Vector2( interpolatedDir.r, interpolatedDir.i ) * interpolatedLength;
}

void Player::UpdateDebugStateFromInput()
{
	if( g_theInput->WasKeyJustPressed( 'R' ) )
		m_debugAutoStopCameraReorientation = !m_debugAutoStopCameraReorientation;
}

void Player::UpdateCameraForward( CameraState const &currentCamState )
{
	if( m_movementInputState == INPUT_LOCKED )
		m_cameraForward = m_cameraStateOnInputLocked.GetTransformMatrix().GetKColumn();
	else
		m_cameraForward = currentCamState.GetTransformMatrix().GetKColumn();
}

void Player::LockInputState( CameraState const &camState )
{
	if( m_inputInterpolationDisabled )
		return;

	m_movementInputState = INPUT_LOCKED;

	m_cameraStateOnInputLocked = camState;
	m_leftStickOnInputLocked = g_theInput->m_controller[0].m_xboxStickStates[XBOX_STICK_LEFT].correctedNormalizedPosition;
}

void Player::StartInputInterpolation( CameraState const &camState )
{
	if( m_inputInterpolationDisabled )
		return;

	m_movementInputState = INPUT_INTERPOLATION;

	// Match the actual camera forward
	m_cameraForward = camState.GetTransformMatrix().GetKColumn();

	// Get the velocity (movement) direction in the Camera Space
	Vector3 velocityDirection	= m_velocity;
	float velotiyLength			= velocityDirection.NormalizeAndGetLength();
	velocityDirection			= (velotiyLength == 0.f) ? Vector3::ZERO : velocityDirection;

	Matrix44 cameraTransformMat	= camState.GetTransformMatrix();
	Matrix44 worldToCameraMatrix;
	
	bool inverseSuccess = cameraTransformMat.GetInverse( worldToCameraMatrix );
	GUARANTEE_RECOVERABLE( inverseSuccess, "Warning: Inverting the camera transform matrix FAILDED!" );

	Vector3 camRelativeVelocityDirection	= worldToCameraMatrix.Multiply( velocityDirection, 0.f );
	Vector2 camRelativeVelocityDirectionXZ	= Vector2( camRelativeVelocityDirection.x, camRelativeVelocityDirection.z ).GetNormalized();
	
	// Remap the left stick input with current camera's orientation such that the movement direction remains maintained
	float leftStickNormalizedMagnitude	= m_leftStickOnInputLocked.GetLength();
	m_leftStickOnInputLocked			= camRelativeVelocityDirectionXZ * leftStickNormalizedMagnitude;

	m_inputInterpolationTimer.SetTimer( m_inputInterpolationSeconds );
}

void Player::UnlockInputState()
{
	m_movementInputState = INPUT_UNLOCKED;
}

void Player::ToggleInputInterpolationDisabled()
{
	m_inputInterpolationDisabled = !m_inputInterpolationDisabled;
	m_movementInputState = INPUT_UNLOCKED;
}

void Player::DebugRenderLeftStickInput( Vector2 const &screenPosition, float widthSize ) const
{
	float halfWidth		= widthSize * 0.5f;
	float circleRadius	= halfWidth * 0.95f;
	AABB2 frameBounds	= AABB2( screenPosition, halfWidth, halfWidth );

	// Auto-Stop Camera Reorientation state
	Vector2 autoStopCamReorientationTxtPos = Vector2( frameBounds.mins.x, frameBounds.maxs.y ) + Vector2( 0.f, 40.f );
	DebugRender2DText( 0.f, autoStopCamReorientationTxtPos, 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf( "[R] Auto-Stop Camera Reorientation - \"%s\"", m_debugAutoStopCameraReorientation ? "enabled" : "DISABLED" ) );
	Vector2 inputInterpolationDisabledTxtPos = autoStopCamReorientationTxtPos + Vector2( 0.f, 20.f );
	DebugRender2DText( 0.f, inputInterpolationDisabledTxtPos, 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("[F] Input Interpolation Feature - \"%s\"", m_inputInterpolationDisabled ? "DISABLED" : "enabled" ) );

	// Render the framing
	DebugRender2DQuad( 0.f, frameBounds, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR );
	DebugRender2DRound( 0.f, screenPosition, circleRadius, 25, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR );

	if( m_movementInputState != INPUT_UNLOCKED )
	{
		// Region - Retain Input
		Vector2 leftStickLockedScreenPosition;
		if( m_movementInputState == INPUT_LOCKED )
			leftStickLockedScreenPosition = screenPosition + (m_leftStickOnInputLocked * circleRadius);
		else if( m_movementInputState == INPUT_INTERPOLATION )
			leftStickLockedScreenPosition = screenPosition + (m_leftStickWhenInterpolating * circleRadius);
		
		DebugRender2DRound( 0.f, leftStickLockedScreenPosition, circleRadius * m_retainInputRegionRadiusFraction, 15, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR );

		// Region - Left Stick Released
		DebugRender2DRound( 0.f, screenPosition, circleRadius * m_leftStickReleasedRegionRadiusFraction, 15, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR );

		// Position - Locked Left Stick
		DebugRender2DRound( 0.f, leftStickLockedScreenPosition, 5.f, 10, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR );
	}

	// Render current Left Stick Position
	Vector2 leftStickCurrentPosition	= g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
	Vector2 leftStickCurrentScreenPos	= screenPosition + (leftStickCurrentPosition * circleRadius);
	DebugRender2DRound( 0.f, leftStickCurrentScreenPos, 5.f, 10, RGBA_RED_COLOR, RGBA_RED_COLOR );
}
