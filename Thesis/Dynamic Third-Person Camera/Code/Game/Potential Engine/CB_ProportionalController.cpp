#pragma once
#include "CB_ProportionalController.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Potential Engine/CameraContext.hpp"
#include "Game/Potential Engine/CameraManager.hpp"

CB_ProportionalController::CB_ProportionalController( char const *name, CameraManager const *manager )
	: CB_DegreesOfFreedom( name, manager )
{

}

CB_ProportionalController::~CB_ProportionalController()
{

}

void CB_ProportionalController::PreUpdate()
{
	// Change controlling fraction as per input
	if( g_theInput->IsKeyPressed( UP ) )
		m_controllingFactor += 3.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->IsKeyPressed( DOWN ) )
		m_controllingFactor -= 3.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->IsKeyPressed( RIGHT ) )
		m_accelerationLimit += 3.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->IsKeyPressed( LEFT ) )
		m_accelerationLimit -= 3.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->WasKeyJustPressed( 'M' ) )
		m_mpcEnabled = !m_mpcEnabled;
	if( g_theInput->IsKeyPressed( PAGE_DOWN ) )
		m_leadFactor -= 2.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->IsKeyPressed( PAGE_UP ) )
		m_leadFactor += 2.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );

	// Clamp the factors
	m_controllingFactor	= (m_controllingFactor < 0.f)	? 0.f : m_controllingFactor;
	m_accelerationLimit = (m_accelerationLimit < 0.f )	? 0.f : m_accelerationLimit;
	m_leadFactor		= (m_leadFactor < 0.f)			? 0.f : m_leadFactor;
}

void CB_ProportionalController::PostUpdate()
{
	// Print it on Debug Screen
	std::string pcTypeString = Stringf( "Currently Active: %s", m_mpcEnabled ? "MPC" : "PC" );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_BLUE_COLOR, RGBA_BLUE_COLOR, pcTypeString.c_str() );
	std::string mpcEnableStr = "Press [M] to enable toggle MPC";
	DebugRender2DText( 0.f, Vector2(-850.f, 440.f), 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, mpcEnableStr.c_str() );
	std::string upDownStr = "Press [UP] [DOWN] to change the controlling factor.";
	DebugRender2DText( 0.f, Vector2(-850.f, 420.f), 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, upDownStr.c_str() );
	std::string leftRightStr = "Press [Right] [LEFT] to change the acceleration limit.";
	DebugRender2DText( 0.f, Vector2(-850.f, 400.f), 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, leftRightStr.c_str() );
	std::string trianglesStr = "Press [PG UP] [PG DOWN] to change the lead factor.";
	DebugRender2DText( 0.f, Vector2(-850.f, 380.f), 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, trianglesStr.c_str() );

	// Controlling Factor
	std::string controllingFractionStr = Stringf( "%-20s = %f", "Controlling Factor", m_controllingFactor );
	DebugRender2DText( 0.f, Vector2(-850.f, 360.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, controllingFractionStr.c_str() );

	// Acceleration Limit
	std::string accelerationLimitStr = Stringf( "%-20s = %f", "Acceleration Limit", m_accelerationLimit );
	DebugRender2DText( 0.f, Vector2(-850.f, 340.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, accelerationLimitStr.c_str() );

	// Lead Factor
	std::string leadFactorStr = Stringf( "%-20s = %f", "Lead Factor", m_leadFactor );
	DebugRender2DText( 0.f, Vector2(-850.f, 320.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, leadFactorStr.c_str() );
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
	Vector3 suggestVelocity	= diffInPosition * m_controllingFactor;
	if( m_mpcEnabled )
	{
		// Modified PC
		CameraContext context = m_manager->GetCameraContext();
		suggestVelocity += (context.anchorGameObject->m_velocity) * m_leadFactor;
	}

	// Control exit characteristics
	Vector3 deltaVelocity;
	Vector3 differenceInVelocity = suggestVelocity - currentState.m_velocity;
	float	maxDeltaVelocity	 = m_accelerationLimit * deltaSeconds;
	float	velocityDiffLength	 = differenceInVelocity.GetLength();
	if( velocityDiffLength < maxDeltaVelocity )
	{
		deltaVelocity = differenceInVelocity;
	}
	else
	{
		deltaVelocity = (differenceInVelocity / velocityDiffLength) * maxDeltaVelocity;
	}

	// Set the final state
	CameraState finalState( m_goalState );
	finalState.m_position = currentState.m_position;
	finalState.m_velocity = currentState.m_velocity + deltaVelocity;

	// Move according to velocity, too!
	finalState.m_position += finalState.m_velocity * deltaSeconds;

	return finalState;
}
