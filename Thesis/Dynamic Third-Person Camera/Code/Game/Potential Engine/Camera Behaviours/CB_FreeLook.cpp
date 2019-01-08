#pragma once
#include "CB_FreeLook.hpp"
#include "Engine/Core/Window.hpp"

CB_FreeLook::CB_FreeLook( float movementSpeed, float rotationSpeed, float minPitchAngle, float maxPitchAngle, char const *behaviourName, CameraManager const *manager, eFreeLookInputSource inputSource )
	: CameraBehaviour( behaviourName, manager )
	, m_movementSpeed( movementSpeed )
	, m_rotationSpeed( rotationSpeed )
	, m_pitchRange( minPitchAngle, maxPitchAngle )
	, m_inputSource( inputSource )
{
	TODO( "Sync pitchLimits with behaviour CB_DegreesOfFreedom!" );
}

CB_FreeLook::~CB_FreeLook()
{

}

void CB_FreeLook::PreUpdate()
{

}

void CB_FreeLook::PostUpdate()
{

}

CameraState CB_FreeLook::Update( float deltaSeconds, CameraState const &currentState )
{
	UNUSED( currentState );

	float	yAxisChange		 = 0.f;
	Vector2	xzMovementChange = Vector2::ZERO;
	Vector2	xyRotationChange = Vector2::ZERO;
	
	// Process Input
	ProcessInput( xzMovementChange, xyRotationChange, yAxisChange );

	// Position Change
	Vector3  cameraPosition	 = m_camera->m_cameraTransform.GetWorldPosition();
	Matrix44 cameraTransform = m_camera->m_cameraTransform.GetWorldTransformMatrix();
	Vector3  cameraForward	 = cameraTransform.GetKColumn();
	
	// We wanna move in the XZ-Plane, no Y-component
	cameraForward.y = 0.f;
	if( cameraForward.GetLength() == 0.f )
		cameraForward = Vector3::FRONT;
	cameraForward = cameraForward.GetNormalized();
	
	Vector3 cameraRight = Vector3::CrossProduct( Vector3::UP, cameraForward );
	cameraPosition += ( (cameraForward * xzMovementChange.y) + (cameraRight * xzMovementChange.x) + Vector3( 0.f, yAxisChange, 0.f ) ) * m_movementSpeed * deltaSeconds;
	
	// Orientation Change
	Vector3 cameraOrientation = m_camera->m_cameraTransform.GetRotation();
	cameraOrientation.z  = 0.f;
	cameraOrientation.y +=  1.f * xyRotationChange.x * m_rotationSpeed * deltaSeconds;
	cameraOrientation.x += -1.f * xyRotationChange.y * m_rotationSpeed * deltaSeconds;

	// Clamp the Pitch
	cameraOrientation.x = fmodf( cameraOrientation.x, 360.f );
	cameraOrientation.x = ClampFloat( cameraOrientation.x, m_pitchRange.min, m_pitchRange.max );

	// FOV Change
	float fov = m_camera->GetFOV();

	return CameraState( Vector3::ZERO, cameraPosition, cameraOrientation, fov );
}

void CB_FreeLook::ProcessInput( Vector2 &outXZMovement, Vector2 &outXYRotation, float &outYAxisChange )
{
	float	yAxisChange		 = 0.f;
	Vector2	xzMovementChange = Vector2::ZERO;
	Vector2 xyRotationChange = Vector2::ZERO;

	if( m_inputSource == USE_CONTROLLER_FL )
	{
		// Input from Xbox Controller
		XboxController &controller	 = m_inputSystem->m_controller[0];
		float			leftTrigger	 = controller.m_xboxTriggerStates[ XBOX_TRIGGER_LEFT ];
		float			rightTrigger = controller.m_xboxTriggerStates[ XBOX_TRIGGER_RIGHT ];
		
		yAxisChange		 = leftTrigger - rightTrigger;
		xzMovementChange = controller.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
		xyRotationChange = controller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;
	}
	else
	{
		// Input from Keyboard-Mouse
		if( g_theInput->IsKeyPressed( 'Q' ) )
			yAxisChange += 1.f;
		if( g_theInput->IsKeyPressed( 'E' ) )
			yAxisChange -= 1.f;

		if( g_theInput->IsKeyPressed( 'W' ) )
			xzMovementChange.y += 1.f;
		if( g_theInput->IsKeyPressed( 'S' ) )
			xzMovementChange.y -= 1.f;
		if( g_theInput->IsKeyPressed( 'A' ) )
			xzMovementChange.x -= 1.f;
		if( g_theInput->IsKeyPressed( 'D' ) )
			xzMovementChange.x += 1.f;

		float sensitivity = 0.2f;
		Vector2 deltaMouse = g_theInput->GetMouseDelta();
		xyRotationChange.x = deltaMouse.x * sensitivity;
		xyRotationChange.y = deltaMouse.y * sensitivity * -1.f;
	}

	// Set out variables
	outYAxisChange	= yAxisChange;
	outXZMovement	= xzMovementChange;
	outXYRotation	= xyRotationChange;
}
