#pragma once
#include "CB_Follow.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

CB_Follow::CB_Follow( float distFromAnchor, float rotationSpeed, float minPitchAngle, float maxPitchAnngle, char const *name, CameraManager const *manager )
	: CB_DegreesOfFreedom( name, manager )
	, m_rotationSpeed( rotationSpeed )
	, m_pitchRange( minPitchAngle, maxPitchAnngle )
	, m_maxDistFromAnchor( distFromAnchor )
	, m_distanceFromAnchor( distFromAnchor )
{

}

CB_Follow::~CB_Follow()
{

}

CameraState CB_Follow::Update( float deltaSeconds, CameraState const &currentState )
{
	UNUSED( currentState );

	// Get input from Xbox Controller
	XboxController &controller = m_inputSystem->m_controller[0];
	
	Vector2 rightStick = controller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;		// For Rotation
	
	bool leftShoulderPressed  = controller.m_xboxButtonStates[ XBOX_BUTTON_LB ].keyIsDown;					// For change in Distance from Anchor
	bool rightShoulderPressed = controller.m_xboxButtonStates[ XBOX_BUTTON_RB ].keyIsDown;
	
	bool dPadUp		= controller.m_xboxButtonStates[ XBOX_BUTTON_UP ].keyIsDown;							// For Offset change
	bool dPadDown	= controller.m_xboxButtonStates[ XBOX_BUTTON_DOWN ].keyIsDown;
	bool dPadRight	= controller.m_xboxButtonStates[ XBOX_BUTTON_RIGHT ].keyIsDown;
	bool dPadLeft	= controller.m_xboxButtonStates[ XBOX_BUTTON_LEFT ].keyIsDown;

	float leftTrigger  = controller.m_xboxTriggerStates[ XBOX_TRIGGER_LEFT ];								// For the FOV change
	float rightTrigger = controller.m_xboxTriggerStates[ XBOX_TRIGGER_RIGHT ];
	

	// Distance from Anchor
	float distanceChange	 = 0.f;
	distanceChange			+= rightTrigger * -1.f * m_distanceChangeSpeed * deltaSeconds;
	distanceChange			+= leftTrigger  *  1.f * m_distanceChangeSpeed * deltaSeconds;
	m_distanceFromAnchor	+= distanceChange;

	// Altitude & Rotation
	float rotationChange	 = -1.f * rightStick.x * m_rotationSpeed * deltaSeconds;
	float altitudeChange	 = -1.f * rightStick.y * m_rotationSpeed * deltaSeconds;
	m_rotationAroundAnchor	+= rotationChange;
	m_altitudeAroundAnchor	+= altitudeChange;

	// Clamp the Altitude
	float clampedAltitude	= ClampFloat( m_altitudeAroundAnchor, m_pitchRange.min, m_pitchRange.max );
	m_altitudeAroundAnchor	= clampedAltitude;

	// Vertical Offset
	float verticalOffsetChange	 = 0.f;
	verticalOffsetChange		+= dPadUp	? (  1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;
	verticalOffsetChange		+= dPadDown ? ( -1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;
	m_localVerticalOffset		+= verticalOffsetChange;

	// Horizontal Offset
	float horizontalOffsetChange = 0.f;
	horizontalOffsetChange		+= dPadRight ? (  1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;
	horizontalOffsetChange		+= dPadLeft  ? ( -1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;
	m_localHorizontalOffset		+= horizontalOffsetChange;

	// Field of View
	m_fov += leftShoulderPressed  ?  1.f * m_fovChangeSpeed * deltaSeconds : 0.f;
	m_fov += rightShoulderPressed ? -1.f * m_fovChangeSpeed * deltaSeconds : 0.f;


	// Set the current CameraState
	SetWorldPosition( m_distanceFromAnchor, m_rotationAroundAnchor, m_altitudeAroundAnchor );
	SetOrientationToLookAtAnchor();
	SetOffsetToWorldPosition( m_localHorizontalOffset, m_localVerticalOffset );
	SetFOV( m_fov );
	
	return m_goalState;
}

void CB_Follow::SuggestChangedPolarCoordinate( float radius, float rotation, float altitude )
{
	UNUSED( radius );
	m_rotationAroundAnchor = rotation;
	m_altitudeAroundAnchor = altitude;
}

void CB_Follow::CartesianToPolarTest( CameraState const &camState ) const
{
	Vector3 anchorPos	= m_manager->GetCameraContext().anchorGameObject->m_transform.GetWorldPosition();
	Vector3 position	= camState.m_position;
	
	float radius, rotation, altitude;
	CartesianToPolar( position - anchorPos, radius, rotation, altitude );
	DebuggerPrintf( "\n Current    Polar: Radius: %f, Rotation: %f, Altitude: %f", m_distanceFromAnchor, m_rotationAroundAnchor, m_altitudeAroundAnchor );
	DebuggerPrintf( "\n Calculated Polar: Radius: %f, Rotation: %f, Altitude: %f", radius, rotation, altitude );

	Vector3 cartPosition = PolarToCartesian( radius, rotation, altitude );

	DebugRenderPoint( 0.f, 1.f, anchorPos, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, DEBUG_RENDER_XRAY );
	DebugRenderWireSphere( 3.f, anchorPos + cartPosition, 0.2f, RGBA_GREEN_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_XRAY );
}
