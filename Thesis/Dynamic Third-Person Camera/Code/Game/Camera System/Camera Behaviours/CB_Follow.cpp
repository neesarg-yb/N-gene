#pragma once
#include "CB_Follow.hpp"
#include "Engine/Math/Complex.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/GameCommon.hpp"

CB_Follow::CB_Follow( float distFromAnchor, float rotationSpeed, float minPitchAngle, float maxPitchAnngle, char const *name, CameraManager const *manager )
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
	// Contextual Info.
	CameraContext context = m_manager->GetCameraContext();

	// Controller input
	float distChangePerInput, rotChangePerInput, altChangePerInput, hOffsetChangePerInput, vOffsetChangePerInput, fovChangePerInput;
	GetPlayerInput( distChangePerInput, rotChangePerInput, altChangePerInput, hOffsetChangePerInput, vOffsetChangePerInput, fovChangePerInput );

	// Scripted Reorient Camera Behavior
	UNUSED( currentState );
//	CheckEnableCameraReorientation( currentState, context, rotChangePerInput );

	if( m_reorientCameraRotation )
	{
		Vector3	playerFront			= context.anchorGameObject->m_transform.GetWorldTransformMatrix().GetKColumn();
		Vector2	playerFrontDirXZ	= Vector2( playerFront.x, playerFront.z ).GetNormalized();
		float	targetDegrees		= GetRotationToFaceXZDirection( playerFrontDirXZ ) - 180.f;		// -180 because we want to set rotation such that the camera is on BACK-SIDE of the player
		Complex targetRot( targetDegrees );

		Complex currentRot( m_rotationAroundAnchor );
		currentRot.TurnToward( targetRot, m_rotationSpeed * 2.f * deltaSeconds );
		
		m_rotationAroundAnchor = currentRot.GetRotation();

		// Done reorienting the camera, if near to the target rotation..
		Complex pendingRot = targetRot / currentRot;
		if( fabsf( pendingRot.GetRotation() ) <= 5.f )
			m_reorientCameraRotation = false;
	}
	else
		m_rotationAroundAnchor	+= rotChangePerInput * m_rotationSpeed * deltaSeconds;

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
	
	return m_goalState;
}

void CB_Follow::SuggestChangedPolarCoordinate( float radius, float rotation, float altitude )
{
	UNUSED( radius );
	m_rotationAroundAnchor = rotation;
	m_altitudeAroundAnchor = altitude;
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

void CB_Follow::CheckEnableCameraReorientation( CameraState const &currentState, CameraContext const &context, float rotationChangeInput )
{
	Matrix44		cameraMat	= currentState.GetTransformMatrix();
	Vector3			playerFront	= context.anchorGameObject->m_transform.GetWorldTransformMatrix().GetKColumn();
	float			playerSpeed	= context.anchorGameObject->m_velocity.GetLength();

	Vector3 cameraFront		 = cameraMat.GetKColumn();
	Vector2 playerFrontDirXZ = Vector2( playerFront.x, playerFront.z ).GetNormalized();
	Vector2 cameraFrontDirXZ = Vector2( cameraFront.x, cameraFront.z ).GetNormalized();
	bool noPlayerInputRot	 = AreEqualFloats( fabsf(rotationChangeInput), 0.f, 2 );

	if( noPlayerInputRot )
	{
		if( m_reorientCameraRotation == false )
		{
			float dotProduct = Vector2::DotProduct( playerFrontDirXZ, cameraFrontDirXZ );
			bool angleThresholdIsCrossed = dotProduct <= m_reorientDotThreshold;
			bool playerHasEnoughSpeed	 = playerSpeed >= m_minSpeedReqToReorient;

			if( playerHasEnoughSpeed && angleThresholdIsCrossed )
			{
				// Enable scripted reorientation behavior
				m_reorientCameraRotation = true;
			}
		}
	}
	else
	{
		// Player input overrides the scripted behavior
		m_reorientCameraRotation = false;
	}


	// Manually triggers the reorientation when (B) button is pressed
//
// 	if( g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_B ].keyJustPressed )
// 	{
// 		if( m_reorientCameraRotation == false )
// 			m_reorientCameraRotation = true;
// 	}
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
