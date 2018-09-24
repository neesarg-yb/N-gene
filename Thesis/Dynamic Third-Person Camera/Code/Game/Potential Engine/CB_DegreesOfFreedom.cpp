#pragma once
#include "CB_DegreesOfFreedom.hpp"

CB_DegreesOfFreedom::CB_DegreesOfFreedom( float distFromAnchor, float rotationSpeed, float minPitchAngle, float maxPitchAnngle, char const *name /* = "DegreesOfFreedom" */ )
	: CameraBehaviour( name )
	, m_rotationSpeed( rotationSpeed )
	, m_pitchRange( minPitchAngle, maxPitchAnngle )
{
	// Radius of spherical coordinate == Distance from Anchor
	m_spehicalCoordinates.x = distFromAnchor;
}

CB_DegreesOfFreedom::~CB_DegreesOfFreedom()
{

}

CameraTargetPoint CB_DegreesOfFreedom::Update( float deltaSeconds )
{
	// Get input from Xbox Controller
	XboxController &controller	= m_inputSystem->m_controller[0];
	Vector2 rightStick			= controller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;
	bool upButtonPressed		= controller.m_xboxButtonStates[ XBOX_BUTTON_UP ].keyIsDown;
	bool downButtonPressed		= controller.m_xboxButtonStates[ XBOX_BUTTON_DOWN ].keyIsDown;

	float distanceChange = 0.f;
	distanceChange += upButtonPressed	? ( -1.f * m_distanceChangeSpeed * deltaSeconds ) : 0.f;
	distanceChange += downButtonPressed	? (  1.f * m_distanceChangeSpeed * deltaSeconds ) : 0.f;

	float rotationChange = rightStick.x * m_rotationSpeed * deltaSeconds;
	float altitudeChange = rightStick.y * m_rotationSpeed * deltaSeconds;

	// Calculate Camera's Position
	Vector3 anchorWorldPosition		= m_anchor->m_transform.GetWorldPosition();
	float	clampedAltitude			= ClampFloat( m_spehicalCoordinates.z + altitudeChange, m_pitchRange.min, m_pitchRange.max );
	Vector3 relativeCameraPosition	= GetPositionFromSpericalCoordinate( m_spehicalCoordinates.x + distanceChange, m_spehicalCoordinates.y + rotationChange, clampedAltitude );
	Vector3 worldCameraPosition		= anchorWorldPosition + relativeCameraPosition;

	// Calculate Camera's Orientation
	Matrix44	lookAtAnchorMatrix	= Matrix44::MakeLookAtView( anchorWorldPosition, worldCameraPosition );
	Quaternion	cameraOrientation	= Quaternion::FromMatrix( lookAtAnchorMatrix );

	return CameraTargetPoint( worldCameraPosition, cameraOrientation, 45.f );
}

Vector3 CB_DegreesOfFreedom::GetPositionFromSpericalCoordinate( float radius, float rotation, float altitude )
{
	UNUSED( radius );
	UNUSED( rotation );
	UNUSED( altitude );

	return Vector3();
}
