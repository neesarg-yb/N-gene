#pragma once
#include "CB_DegreesOfFreedom.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

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
	// For Rotation of the Camera
	Vector2 rightStick			= controller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;
	// For change in Distance from Anchor
	bool leftShoulderPressed	= controller.m_xboxButtonStates[ XBOX_BUTTON_LB ].keyIsDown;
	bool rightShoulder			= controller.m_xboxButtonStates[ XBOX_BUTTON_RB ].keyIsDown;
	// For Offset change
	bool dPadUp					= controller.m_xboxButtonStates[ XBOX_BUTTON_UP ].keyIsDown;
	bool dPadDown				= controller.m_xboxButtonStates[ XBOX_BUTTON_DOWN ].keyIsDown;
	bool dPadRight				= controller.m_xboxButtonStates[ XBOX_BUTTON_RIGHT ].keyIsDown;
	bool dPadLeft				= controller.m_xboxButtonStates[ XBOX_BUTTON_LEFT ].keyIsDown;
	// For the FOV change
	float leftTrigger			= controller.m_xboxTriggerStates[ XBOX_TRIGGER_LEFT ];
	float rightTrigger			= controller.m_xboxTriggerStates[ XBOX_TRIGGER_RIGHT ];

	float distanceChange = 0.f;
	distanceChange += rightShoulder			? ( -1.f * m_distanceChangeSpeed * deltaSeconds ) : 0.f;
	distanceChange += leftShoulderPressed	? (  1.f * m_distanceChangeSpeed * deltaSeconds ) : 0.f;

	float rotationChange =  1.f * rightStick.x * m_rotationSpeed * deltaSeconds;
	float altitudeChange = -1.f * rightStick.y * m_rotationSpeed * deltaSeconds;
	
	float verticalOffsetChange = 0.f;
	verticalOffsetChange += dPadUp	 ? (  1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;
	verticalOffsetChange += dPadDown ? ( -1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;

	float horizontalOffsetChange = 0.f;
	horizontalOffsetChange += dPadRight	? (  1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;
	horizontalOffsetChange += dPadLeft	? ( -1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;

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

	// Offset from Center
	Vector3		rightOfCameraInWorld = cameraOrientation.RotatePoint( Vector3::RIGHT );
	Vector3		upOfCameraInWorld	 = cameraOrientation.RotatePoint( Vector3::UP );
				m_offsetFromCenter	+= Vector2( horizontalOffsetChange, verticalOffsetChange );
	Vector3		worldPositionOffset	 = ( rightOfCameraInWorld * m_offsetFromCenter.x ) + ( upOfCameraInWorld * m_offsetFromCenter.y );

	// Field of View
	float cameraFOV = m_camera->GetFOV();
	cameraFOV += ( leftTrigger - rightTrigger ) * m_fovChangeSpeed * deltaSeconds;

	return CameraTargetPoint( worldCameraPosition + worldPositionOffset, cameraOrientation, cameraFOV );
}

Vector3 CB_DegreesOfFreedom::GetPositionFromSpericalCoordinate( float radius, float rotation, float altitude )
{
	float roationRelativeToXAxis = rotation - 90.f;

	return PolarToCartesian( radius, roationRelativeToXAxis, altitude);
}
