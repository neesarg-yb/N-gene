#pragma once
#include "CB_FreeLook.hpp"

CB_FreeLook::CB_FreeLook( float movementSpeed, float rotationSpeed, float minPitchAngle, float maxPitchAngle, char const *behaviourName )
	: CameraBehaviour( behaviourName )
	, m_movementSpeed( movementSpeed )
	, m_rotationSpeed( rotationSpeed )
	, m_pitchRange( minPitchAngle, maxPitchAngle )
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

CameraTargetPoint CB_FreeLook::Update( float deltaSeconds )
{
	// Input from Xbox Controller
	XboxController &controller	 = m_inputSystem->m_controller[0];
	Vector2			leftStick	 = controller.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
	Vector2			rightStick	 = controller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;
	float			leftTrigger	 = controller.m_xboxTriggerStates[ XBOX_TRIGGER_LEFT ];
	float			rightTrigger = controller.m_xboxTriggerStates[ XBOX_TRIGGER_RIGHT ];
	float			yAxisChange	 = leftTrigger - rightTrigger;

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
	cameraPosition += ( (cameraForward * leftStick.y) + (cameraRight * leftStick.x) + Vector3( 0.f, yAxisChange, 0.f ) ) * m_movementSpeed * deltaSeconds;
	
	// Orientation Change
	Vector3 cameraOrientation = m_camera->m_cameraTransform.GetRotation();
	cameraOrientation.z  = 0.f;
	cameraOrientation.y +=  1.f * rightStick.x * m_rotationSpeed * deltaSeconds;
	cameraOrientation.x += -1.f * rightStick.y * m_rotationSpeed * deltaSeconds;

	// Clamp the Pitch
	cameraOrientation.x = fmodf( cameraOrientation.x, 360.f );
	cameraOrientation.x = ClampFloat( cameraOrientation.x, m_pitchRange.min, m_pitchRange.max );

	// FOV Change
	float fov = m_camera->GetFOV();

	return CameraTargetPoint( cameraPosition, cameraOrientation, fov );
}
