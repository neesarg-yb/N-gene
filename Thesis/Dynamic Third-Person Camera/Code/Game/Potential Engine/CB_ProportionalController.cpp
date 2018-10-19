#pragma once
#include "CB_ProportionalController.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

CB_ProportionalController::CB_ProportionalController( char const *name )
	: CB_DegreesOfFreedom( name )
{

}

CB_ProportionalController::~CB_ProportionalController()
{

}

void CB_ProportionalController::PreUpdate()
{
	// Change controlling fraction as per input
	if( g_theInput->IsKeyPressed( UP ) )
		m_controllingFraction += 3.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->IsKeyPressed( DOWN ) )
		m_controllingFraction -= 3.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
}

void CB_ProportionalController::PostUpdate()
{
	// Print it on Debug Screen
	std::string controllingFractionStr = Stringf( "Controlling Fraction(PC) = %f", m_controllingFraction );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, controllingFractionStr.c_str() );
}

CameraState CB_ProportionalController::Update( float deltaSeconds, CameraState const &currentState )
{
	// Get input from Xbox Controller
	XboxController &controller = m_inputSystem->m_controller[0];

	Vector2 rightStick = controller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;		// For Rotation

	bool leftShoulderPressed	= controller.m_xboxButtonStates[ XBOX_BUTTON_LB ].keyIsDown;				// For change in Distance from Anchor
	bool rightShoulderPressed	= controller.m_xboxButtonStates[ XBOX_BUTTON_RB ].keyIsDown;

	bool dPadUp		= controller.m_xboxButtonStates[ XBOX_BUTTON_UP ].keyIsDown;							// For Offset change
	bool dPadDown	= controller.m_xboxButtonStates[ XBOX_BUTTON_DOWN ].keyIsDown;
	bool dPadRight	= controller.m_xboxButtonStates[ XBOX_BUTTON_RIGHT ].keyIsDown;
	bool dPadLeft	= controller.m_xboxButtonStates[ XBOX_BUTTON_LEFT ].keyIsDown;

	float leftTrigger	= controller.m_xboxTriggerStates[ XBOX_TRIGGER_LEFT ];								// For the FOV change
	float rightTrigger	= controller.m_xboxTriggerStates[ XBOX_TRIGGER_RIGHT ];


	// Distance from Anchor
	float distanceChange	 = 0.f;
	distanceChange			+= rightTrigger * -1.f * m_distanceChangeSpeed * deltaSeconds;
	distanceChange			+= leftTrigger  *  1.f * m_distanceChangeSpeed * deltaSeconds;
	m_distanceFromAnchor	+= distanceChange;

	// Altitude & Rotation
	float rotationChange	 =  1.f * rightStick.x * m_rotationSpeed * deltaSeconds;
	float altitudeChange	 = -1.f * rightStick.y * m_rotationSpeed * deltaSeconds;
	m_rotationAroundAnchor	+= rotationChange;
	m_altitudeAroundAnchor	+= altitudeChange;

	// Clamp the Altitude
	float clampedAltitude	= ClampFloat( m_altitudeAroundAnchor, m_pitchRange.min, m_pitchRange.max );
	m_altitudeAroundAnchor	= clampedAltitude;

	// Vertical Offset
	float verticalOffsetChange	 = 0.f;
	verticalOffsetChange		+= dPadUp	 ? (  1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;
	verticalOffsetChange		+= dPadDown ? ( -1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;
	m_localVerticalOffset		+= verticalOffsetChange;

	// Horizontal Offset
	float horizontalOffsetChange = 0.f;
	horizontalOffsetChange		+= dPadRight	? (  1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;
	horizontalOffsetChange		+= dPadLeft	? ( -1.f * m_offsetChangeSpeed * deltaSeconds ) : 0.f;
	m_localHorizontalOffset		+= horizontalOffsetChange;

	// Field of View
	m_fov += leftShoulderPressed  ?  1.f * m_fovChangeSpeed * deltaSeconds : 0.f;
	m_fov += rightShoulderPressed ? -1.f * m_fovChangeSpeed * deltaSeconds : 0.f;


	// Set the current CameraState
	SetWorldPosition( m_distanceFromAnchor, m_rotationAroundAnchor, m_altitudeAroundAnchor );
	SetOrientationToLookAtAnchor();
	SetOffsetToWorldPosition( m_localHorizontalOffset, m_localVerticalOffset );
	SetFOV( m_fov );


	// Proportional Controller
	Vector3 diffInPosition	= m_goalState.m_position - currentState.m_position;
	Vector3 suggestVelocity	= diffInPosition * m_controllingFraction;

	CameraState goalStateWithVelocity( m_goalState );
	goalStateWithVelocity.m_position = currentState.m_position;		// Do not move directly, let velocity move position
	goalStateWithVelocity.m_velocity = suggestVelocity;

	return goalStateWithVelocity;
}
