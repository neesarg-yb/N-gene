#pragma once
#include "CB_ShoulderView.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/GameCommon.hpp"

CB_ShoulderView::CB_ShoulderView( float heightRelativeToAnchorPos, float radiusFromAnchor, float localCameraOffsetX, float minPitchOffset, float maxPitchOffset, char const *name, CameraManager *manager )
	: CameraBehaviour( name, manager )
	, m_heightFromAnchorPosition( heightRelativeToAnchorPos )
	, m_radiusFromAnchor( radiusFromAnchor )
	, m_localCameraOffsetX( localCameraOffsetX )
	, m_cameraPitchOffsetLimits( minPitchOffset, maxPitchOffset )
{

}

CB_ShoulderView::~CB_ShoulderView()
{

}

void CB_ShoulderView::PreUpdate()
{

}

void CB_ShoulderView::PostUpdate()
{
	DebugPrintInputInformation();
}

CameraState CB_ShoulderView::Update( float deltaSeconds, CameraState const &currentState )
{
	ProcessInput( deltaSeconds );

	// Contextual Info.
	CameraContext	context			= m_manager->GetCameraContext();
	Vector3			anchorWorldPos	= context.anchorGameObject->m_transform.GetWorldPosition();

	// Get Polar Coordinates
	float radius	= m_radiusFromAnchor;
	float rotation	= m_rotationAroundAnchor;
	float altitude	= 90.f;

	// Get World Position
	Vector3 cameraPosRelativeToAnchor		 = PolarToCartesian( radius, rotation, altitude );
	Vector3 cameraWorldPositionWithoutHeight = anchorWorldPos + cameraPosRelativeToAnchor;
	Vector3 cameraWorldPosition				 = cameraWorldPositionWithoutHeight  + Vector3( 0.f, m_heightFromAnchorPosition, 0.f ); 

	// Get World Orientation
	Matrix44	lookAtViewMatrix	= Matrix44::MakeLookAtView( anchorWorldPos, cameraWorldPositionWithoutHeight );
	Quaternion	cameraOrientation	= Quaternion::FromMatrix( lookAtViewMatrix ).GetInverse();

	// Camera Right Direction
	Matrix44	cameraOrientationMatrix	= cameraOrientation.GetAsMatrix44();
	Vector3		cameraRightDirection	= cameraOrientationMatrix.GetIColumn();

	// Account for local x-offset (in right direction)
	cameraWorldPosition += cameraRightDirection * m_localCameraOffsetX;

	// Apply local pitch offset
	Quaternion localPitchRotation = Quaternion( cameraRightDirection, m_localPitchOffset );
	cameraOrientation = cameraOrientation.Multiply( localPitchRotation );

	// Return the new state
	CameraState newCameraState		= currentState;
	newCameraState.m_position		= cameraWorldPosition;
	newCameraState.m_orientation	= cameraOrientation;

	return newCameraState;
}

void CB_ShoulderView::SetupForIncomingHandover( float rotationAroundAnchor, bool isOnRightShoulder )
{
	m_rotationAroundAnchor	= rotationAroundAnchor;
	
	if( isOnRightShoulder )
	{
		// Offset should be on RIGHT SHOULDER
		bool currentlyOnLeftShoulder = m_localCameraOffsetX < 0.f;
		m_localCameraOffsetX *= currentlyOnLeftShoulder ? -1.f : 1.f;
	}
	else
	{
		// Offset should be on LEFT SHOULDER
		bool currentlyOnRightShoulder = m_localCameraOffsetX > 0.f;
		m_localCameraOffsetX *= currentlyOnRightShoulder ? -1.f : 0.f;
	}
}

void CB_ShoulderView::ProcessInput( float deltaSeconds )
{
	float const offsetChangeSpeed = 0.5f;

	// Left-Right for changing the x-offset
	bool isLeftKeyPressed	= g_theInput->IsKeyPressed( VK_Codes::LEFT );
	bool isRightKeyPressed	= g_theInput->IsKeyPressed( VK_Codes::RIGHT );

	// PG UP-DOWN for changing the height
	bool isPgUpKeyPressed	= g_theInput->IsKeyPressed( VK_Codes::PAGE_UP );
	bool isPgDownKeyPressed	= g_theInput->IsKeyPressed( VK_Codes::PAGE_DOWN );

	// Up-Down keys for changing the radius from anchor
	bool isUpKeyPressed		= g_theInput->IsKeyPressed( VK_Codes::UP );
	bool isDownKeyPressed	= g_theInput->IsKeyPressed( VK_Codes::DOWN );

	// Right Stick button -> Switch the shoulder
	bool wasRightStickJustPressed = g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_RS ].keyJustPressed;

	// Stick input for changing the rotation around anchor
	Vector2 rightStickPosition = g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;

	if( isLeftKeyPressed )
		m_localCameraOffsetX -= offsetChangeSpeed * deltaSeconds;
	if( isRightKeyPressed )
		m_localCameraOffsetX += offsetChangeSpeed * deltaSeconds;
	if( isPgUpKeyPressed )
		m_heightFromAnchorPosition += offsetChangeSpeed * deltaSeconds;
	if( isPgDownKeyPressed )
		m_heightFromAnchorPosition -= offsetChangeSpeed * deltaSeconds;
	if( isUpKeyPressed )
		m_radiusFromAnchor -= offsetChangeSpeed * deltaSeconds;
	if( isDownKeyPressed )
		m_radiusFromAnchor += offsetChangeSpeed * deltaSeconds;
	if( wasRightStickJustPressed )
		m_localCameraOffsetX *= -1.f;

	m_rotationAroundAnchor	+= rightStickPosition.x * -1.f * m_rotationChangeSpeed * deltaSeconds;
	m_localPitchOffset		+= rightStickPosition.y * +1.f * (m_rotationChangeSpeed * 0.7f) * deltaSeconds;
	m_localPitchOffset = ClampFloat( m_localPitchOffset, m_cameraPitchOffsetLimits.min, m_cameraPitchOffsetLimits.max );
}

void CB_ShoulderView::DebugPrintInputInformation() const
{
	// Print it on Debug Screen
	std::string rightStickPressStr = "Controller (RS) Button : switch to the other shoulder";
	DebugRender2DText( 0.f, Vector2(-850.f, 380.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, rightStickPressStr.c_str() );
	std::string pgKeyPressStr = "[PG DOWN] [PG UP] : Move camera vertically";
	DebugRender2DText( 0.f, Vector2(-850.f, 360.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, pgKeyPressStr.c_str() );
	std::string leftRightPressStr = "[LEFT] [RIGHT]    : Move camera horizontally ";
	DebugRender2DText( 0.f, Vector2(-850.f, 340.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, leftRightPressStr.c_str() );
	std::string upDownPressStr = "[UP] [DOWN]       : Move camera in-and-out";
	DebugRender2DText( 0.f, Vector2(-850.f, 320.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, upDownPressStr.c_str() );
}
