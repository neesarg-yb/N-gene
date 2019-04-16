#pragma once
#include "MCamera.hpp"
#include "Game/World/World.hpp"
#include "Game/World/Player.hpp"

MCamera::MCamera( Renderer &activeRenderer, Clock* parentClock, Player const *anchor, World const *inTheWorld )
	: m_renderer( activeRenderer )
	, m_clock( parentClock )
	, m_anchorPlayer( anchor )
	, m_world( inTheWorld )
{
	m_camera = new Camera();

	m_camera->SetColorTarget( m_renderer.GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( m_renderer.GetDefaultDepthTarget() );

	m_flip.SetIColumn( Vector3( 0.f, 0.f, 1.f) );
	m_flip.SetJColumn( Vector3(-1.f, 0.f, 0.f) );
	m_flip.SetKColumn( Vector3( 0.f, 1.f, 0.f) );
}

MCamera::~MCamera()
{
	delete m_camera;
	m_camera = nullptr;
}

void MCamera::Update()
{
	float deltaSeconds = (float) m_clock.GetFrameDeltaSeconds();

	switch (m_cameraMode)
	{
	case CAMERA_DETATCHED:
		Update_CameraDetatched( deltaSeconds );
		break;

	case CAMERA_1ST_PERSON:
		Update_Camera1stPerson( deltaSeconds );
		break;

	case CAMERA_OVER_THE_SHOULDER:
		Update_CameraOverTheShoulder( deltaSeconds );
		break;

	case CAMERA_FIXED_ANGLE:
		Update_CameraFixedAngle( deltaSeconds );
		break;

	default:
		break;
	}
}

void MCamera::Update_CameraDetatched( float deltaSeconds )
{
	// If input is controlling just the player, return
	if( m_inputControlsCamera == false )
		return;

	// Update according to the input
	Vector2	mouseScreenDelta = g_theInput->GetMouseDelta();
	float	forwardMovement = 0.f;
	float	leftMovement = 0.f;
	float	upMovement = 0.f;
	float	movementSpeedMultiplier = 1.f;

	if( g_theInput->IsKeyPressed( 'W' ) )
		forwardMovement += 1.f;
	if( g_theInput->IsKeyPressed( 'S' ) )
		forwardMovement -= 1.f;
	if( g_theInput->IsKeyPressed( 'A' ) )
		leftMovement += 1.f;
	if( g_theInput->IsKeyPressed( 'D' ) )
		leftMovement -= 1.f;
	if( g_theInput->IsKeyPressed( 'Q' ) )
		upMovement += 1.f;
	if( g_theInput->IsKeyPressed( 'E' ) )
		upMovement -= 1.f;
	if( g_theInput->IsKeyPressed( SHIFT ) )
		movementSpeedMultiplier = m_moveFasterScale;

	// Camera Orientation
	m_yawDegreesAboutZ		-= mouseScreenDelta.x * m_camRotationSpeed;
	m_pitchDegreesAboutY	+= mouseScreenDelta.y * m_camRotationSpeed;

	// Camera Position
	float	const camYaw	 = m_yawDegreesAboutZ;
	Vector3 const forwardDir = Vector3( CosDegree(camYaw), SinDegree(camYaw), 0.f );
	Vector3 const leftDir	 = Vector3( forwardDir.y * -1.f, forwardDir.x, 0.f );
	Vector3 const upDir		 = Vector3( 0.f, 0.f, 1.f );
	
	// Just controlling the camera
	m_position	+= ((forwardDir * forwardMovement * m_cameraFlySpeed * movementSpeedMultiplier * deltaSeconds)
				+   (leftDir * leftMovement * m_cameraFlySpeed * movementSpeedMultiplier * deltaSeconds )
				+   (upDir * upMovement * m_cameraFlySpeed * movementSpeedMultiplier * deltaSeconds ));
}

void MCamera::Update_Camera1stPerson( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	// Update according to the input
	Vector2	mouseScreenDelta = g_theInput->GetMouseDelta();

	// Orientation
	m_yawDegreesAboutZ -= mouseScreenDelta.x * m_camRotationSpeed;
	SetPitchDegreesAboutY( m_pitchDegreesAboutY + (mouseScreenDelta.y * m_camRotationSpeed) );

	// Position
	m_position = m_anchorPlayer->GetEyePosition();
}

void MCamera::Update_CameraOverTheShoulder( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	// Update according to the input
	Vector2	mouseScreenDelta = g_theInput->GetMouseDelta();

	// Camera Orientation
	m_yawDegreesAboutZ -= mouseScreenDelta.x * m_camRotationSpeed;
	SetPitchDegreesAboutY( m_pitchDegreesAboutY + (mouseScreenDelta.y * m_camRotationSpeed) );

	// Position
	Vector3 cameraForward  = GetForwardDirection();
	RaycastResult_MC raycastResult = m_world->Raycast( m_anchorPlayer->GetEyePosition(), -cameraForward, m_overTheShoulderRadius );
	m_position = raycastResult.m_impactPosition + (cameraForward * 0.1f);
}

void MCamera::Update_CameraFixedAngle( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

Camera* MCamera::GetCamera()
{
	RebuildMatrices();
	return m_camera;
}

void MCamera::RebuildMatrices()
{
	// View Matrix
	m_view.SetIdentity();
	m_view.Append( m_flip );
	m_view.RotateX3D( -m_rollDegreesAboutX  );
	m_view.RotateY3D( -m_pitchDegreesAboutY );
	m_view.RotateZ3D( -m_yawDegreesAboutZ );
	m_view.Translate3D( -m_position );

	// Projection Matrix
	m_projection = Matrix44::MakePerspective3D( m_initialFOV, g_aspectRatio, m_cameraNear, m_cameraFar );

	// Hammer the matrices on camera!
	m_camera->SetViewMatrixUnsafe( m_view );
	m_camera->SetProjectionMatrixUnsafe( m_projection );
}

float MCamera::SetPitchDegreesAboutY( float desiredPitch )
{
	m_pitchDegreesAboutY = desiredPitch;
	m_pitchDegreesAboutY = ClampFloat( m_pitchDegreesAboutY, -m_pitchLimit, +m_pitchLimit );

	return m_pitchDegreesAboutY;
}

Vector3 MCamera::GetForwardDirection() const
{
	Matrix44 rotationMatrix;

	rotationMatrix.RotateZ3D( m_yawDegreesAboutZ );
	rotationMatrix.RotateY3D( m_pitchDegreesAboutY );
	rotationMatrix.RotateX3D( m_rollDegreesAboutX );

	return rotationMatrix.Multiply( Vector3( 1.f, 0.f, 0.f ), 0.f );
}
