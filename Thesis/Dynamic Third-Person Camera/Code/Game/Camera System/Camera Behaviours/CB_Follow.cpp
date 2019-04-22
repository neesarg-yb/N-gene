#pragma once
#include "CB_Follow.hpp"
#include "Engine/Math/Complex.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/GameCommon.hpp"

ConstraintSuggestionOverwriteState::ConstraintSuggestionOverwriteState( bool playerHasControl, Vector3 playerPositionOnBegin )
	: m_playerHasCameraControl( playerHasControl )
	, m_playerPositionOnBegin( playerPositionOnBegin )
{

}

CB_Follow::CB_Follow( float distFromAnchor, float rotationSpeed, float minPitchAngle, float maxPitchAnngle, char const *name, CameraManager *manager )
	: CB_DegreesOfFreedom( name, manager )
	, m_rotationSpeed( rotationSpeed )
	, m_pitchRange( minPitchAngle, maxPitchAnngle )
	, m_distanceFromAnchor( distFromAnchor )
{
	Vector2 xOne( 1.f, 0.f );
	Vector2 maxRot( CosDegree(m_reorientAfterDegreeDiff), SinDegree(m_reorientAfterDegreeDiff) );
	m_reorientDotThreshold = Vector2::DotProduct( xOne, maxRot );
}

CB_Follow::~CB_Follow()
{

}

CameraState CB_Follow::Update( float deltaSeconds, CameraState const &currentState )
{
	PROFILE_SCOPE_FUNCTION();

	// Contextual Info.
	CameraContext context = m_manager->GetCameraContext();

	// Controller input
	float distChangePerInput, rotChangePerInput, altChangePerInput, hOffsetChangePerInput, vOffsetChangePerInput, fovChangePerInput;
	GetPlayerInput( distChangePerInput, rotChangePerInput, altChangePerInput, hOffsetChangePerInput, vOffsetChangePerInput, fovChangePerInput );

	if( m_reorientCameraRotation )
	{
		// Do the reorientation
		Complex targetRot( m_reorientTargetRotDegrees );
		Complex currentRot( m_rotationAroundAnchor );
		currentRot.TurnToward( targetRot, m_rotationSpeed * 2.f * deltaSeconds );
		
		m_rotationAroundAnchor = currentRot.GetRotation();

		// Done reorienting the camera, if near to the target rotation..
		Complex pendingRot = targetRot / currentRot;
		if( fabsf( pendingRot.GetRotation() ) <= 2.f )
			m_reorientCameraRotation = false;
	}
	else
	{
		// Change the rotation, normally
		m_rotationAroundAnchor	+= rotChangePerInput * m_rotationSpeed * deltaSeconds;
	}

	// Polar Coordinates
	m_distanceFromAnchor	+= distChangePerInput * m_distanceChangeSpeed * deltaSeconds;
	m_altitudeAroundAnchor	+= altChangePerInput * m_rotationSpeed * deltaSeconds;
	m_altitudeAroundAnchor	 = ClampFloat( m_altitudeAroundAnchor, m_pitchRange.min, m_pitchRange.max );
	SetWorldPosition( m_distanceFromAnchor, m_rotationAroundAnchor, m_altitudeAroundAnchor );
	SetOrientationToLookAtAnchor();
	
	// Local Offsets
	m_localHorizontalOffset	+= hOffsetChangePerInput * m_offsetChangeSpeed * deltaSeconds;
	m_localVerticalOffset	+= vOffsetChangePerInput * m_offsetChangeSpeed * deltaSeconds;
	//	Turned off b/c it won't work well with Modified Cone Raycast
	//--->	SetOffsetToWorldPosition( m_localHorizontalOffset, m_localVerticalOffset );

	// FOV
	m_fov += fovChangePerInput * m_fovChangeSpeed * deltaSeconds;
	SetFOV( m_fov );
	
	// If there's player input
	if( fabsf(altChangePerInput) > 0.f || fabsf(rotChangePerInput) > 0.f )
	{
		// Player takes the control over camera
		m_suggestionOverwriteState.m_playerHasCameraControl	= true;
		m_suggestionOverwriteState.m_playerPositionOnBegin	= context.anchorGameObject->m_transform.GetWorldPosition();
		m_suggestionOverwriteState.m_timeElapsedSecondsIdle	= 0.0;
	}
	else
	{
		// If there's no player input, increase the idle time
		m_suggestionOverwriteState.m_timeElapsedSecondsIdle += deltaSeconds;

		// Check if Camera Constraint should get the total control over camera
		bool turnOnContraintSuggesion = CheckToTurnOnConstraintSuggestions( m_suggestionOverwriteState );

		// Yes
		if( turnOnContraintSuggesion )
		{
			// Register that we're taking away the control from the player
			m_suggestionOverwriteState.m_playerHasCameraControl	= false;
			m_suggestionOverwriteState.m_playerPositionOnBegin	= context.anchorGameObject->m_transform.GetWorldPosition();
		}
	}

	DebugRender2DText( 0.f, Vector2( 0.f, -100.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf("Player has controls: %s", m_suggestionOverwriteState.m_playerHasCameraControl ? "true" : "false" ) );
	DebugRender2DText( 0.f, Vector2( 0.f, -120.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf("Time Elapsed Idle: %.2lf", m_suggestionOverwriteState.m_timeElapsedSecondsIdle ) );

	if( m_suggestionOverwriteState.m_playerHasCameraControl )
		m_goalState.m_constraintType =  APPLY_CORRECTION;
	else
		m_goalState.m_constraintType = (APPLY_CORRECTION | APPLY_SUGGESTION);
	
	return m_goalState;
}

void CB_Follow::SetupForIncomingHandover( float rotationAroundAnchor, float altitudeFromTop )
{
	m_rotationAroundAnchor = rotationAroundAnchor;
	m_altitudeAroundAnchor = altitudeFromTop;
}

void CB_Follow::SuggestChangedPolarCoordinate( float radius, float rotation, float altitude )
{
	UNUSED( radius );
	m_rotationAroundAnchor = rotation;
	m_altitudeAroundAnchor = altitude;
}

bool CB_Follow::StartCameraReorientation()
{
	// Start only if previous reorient operation is finished!
	if( m_reorientCameraRotation == false )
	{
		// Enable reorientation
		m_reorientCameraRotation = true;

		// Set the target degrees
		CameraContext context		= m_manager->GetCameraContext();
		Vector3	playerFront			= context.anchorGameObject->m_transform.GetWorldTransformMatrix().GetKColumn();
		Vector2	playerFrontDirXZ	= Vector2( playerFront.x, playerFront.z ).GetNormalized();
		float	targetDegrees		= GetRotationToFaceXZDirection( playerFrontDirXZ ) - 180.f;		// -180 because we want to set rotation such that the camera is on BACK-SIDE of the player
		m_reorientTargetRotDegrees	= targetDegrees;

		return true;
	}
	else 
	{
		// Last reorientation has not finished, yet..
		// Can not start another one
		return false;
	}
}

void CB_Follow::StopCameraReorientation()
{
	m_reorientCameraRotation = false;
}

void CB_Follow::GetPlayerInput( float &distChange_out, float &rotChange_out, float &altChange_out, float &hOffsetChange_out, float &vOffsetChange_out, float &fovChange_out ) const
{
	// Get input from Xbox Controller
	XboxController &controller = m_inputSystem->m_controller[0];
	
	// For Rotation
	Vector2 rightStick = controller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;

	// For change in Distance from Anchor
	bool leftShoulderPressed  = controller.m_xboxButtonStates[ XBOX_BUTTON_LB ].keyIsDown;
	bool rightShoulderPressed = controller.m_xboxButtonStates[ XBOX_BUTTON_RB ].keyIsDown;

	// For Offset change
	bool dPadUp		= controller.m_xboxButtonStates[ XBOX_BUTTON_UP ].keyIsDown;
	bool dPadDown	= controller.m_xboxButtonStates[ XBOX_BUTTON_DOWN ].keyIsDown;
	bool dPadRight	= controller.m_xboxButtonStates[ XBOX_BUTTON_RIGHT ].keyIsDown;
	bool dPadLeft	= controller.m_xboxButtonStates[ XBOX_BUTTON_LEFT ].keyIsDown;

	// For the FOV change
	float leftTrigger  = controller.m_xboxTriggerStates[ XBOX_TRIGGER_LEFT ];
	float rightTrigger = controller.m_xboxTriggerStates[ XBOX_TRIGGER_RIGHT ];
	
	// Setting the out variables
	float distanceChange = 0.f;
	distanceChange += rightTrigger * -1.f;
	distanceChange += leftTrigger  *  1.f;
	distChange_out  = distanceChange;

	float rotationChange = -1.f * rightStick.x;
	float altitudeChange = -1.f * rightStick.y;
	rotChange_out = rotationChange;
	altChange_out = altitudeChange;

	float horizontalOffsetChange = 0.f;
	horizontalOffsetChange += dPadRight ?  1.f : 0.f;
	horizontalOffsetChange += dPadLeft  ? -1.f : 0.f;
	hOffsetChange_out = horizontalOffsetChange;

	float verticalOffsetChange = 0.f;
	verticalOffsetChange += dPadUp   ?  1.f : 0.f;
	verticalOffsetChange += dPadDown ? -1.f : 0.f;
	vOffsetChange_out = verticalOffsetChange;

	float fovChange = 0.f;
	fovChange += leftShoulderPressed  ?  1.f : 0.f;
	fovChange += rightShoulderPressed ? -1.f : 0.f;
	fovChange_out = fovChange;
}

bool CB_Follow::CheckToTurnOnConstraintSuggestions( ConstraintSuggestionOverwriteState const &suggestionOverwriteState ) const
{
	if( suggestionOverwriteState.m_timeElapsedSecondsIdle >= m_constrainTakeOverTime )
		return true;
	else
		return false;
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

float CB_Follow::GetRotationToFaceXZDirection( Vector2 const &xzDir ) const
{
	// Mult by -1: because orientation from Vec2 is calculated based on Right Handed rotation (counter clock-wise); Our polar coords works based on clock-wise rotation..
	return ( -1.f * xzDir.GetOrientationDegrees() );
}
