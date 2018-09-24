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

	float rotationChange =  1.f * rightStick.x * m_rotationSpeed * deltaSeconds;
	float altitudeChange = -1.f * rightStick.y * m_rotationSpeed * deltaSeconds;

	// Calculate Camera's Position
	Vector3 anchorWorldPosition		= m_anchor->m_transform.GetWorldPosition();
	float	clampedAltitude			= ClampFloat( m_spehicalCoordinates.z + altitudeChange, m_pitchRange.min, m_pitchRange.max );
	m_spehicalCoordinates			+= Vector3( distanceChange, rotationChange, 0.f );
	m_spehicalCoordinates.z			= clampedAltitude;
	Vector3 relativeCameraPosition	= GetPositionFromSpericalCoordinate( m_spehicalCoordinates.x, m_spehicalCoordinates.y, m_spehicalCoordinates.z );
	Vector3 worldCameraPosition		= anchorWorldPosition + relativeCameraPosition;

	// Calculate Camera's Orientation
	Matrix44	lookAtAnchorMatrix	= Matrix44::MakeLookAtView( anchorWorldPosition, worldCameraPosition );
	Quaternion	cameraOrientation	= Quaternion::FromMatrix( lookAtAnchorMatrix ).GetInverse();
	TODO( "Find out: Why cameraOrientation.GetInverse() works?!" );

	return CameraTargetPoint( worldCameraPosition, cameraOrientation, 45.f );
}

Vector3 CB_DegreesOfFreedom::GetPositionFromSpericalCoordinate( float radius, float rotation, float altitude )
{
	float roationRelativeToXAxis = rotation - 90.f;

	return PolarToCartesian( radius, roationRelativeToXAxis, altitude);
}
