#pragma once
#include "CMC_ProportionalController.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/GameCommon.hpp"

CMC_ProportionalController::CMC_ProportionalController( char const *name, CameraManager const *manager )
	: CameraMotionController( name, manager )
{

}

CMC_ProportionalController::~CMC_ProportionalController()
{

}

CameraState CMC_ProportionalController::MoveCamera( CameraState const &currentState, CameraState const &goalState, float deltaSeconds )
{
	DebugRender2DText( 0.f, Vector2( 0.f, 100.f), 15.f, RGBA_YELLOW_COLOR, RGBA_YELLOW_COLOR, Stringf( "Current Cam Velocity: %.1f", currentState.m_velocity.GetLength() ) );

	ProcessInput();
	DebugPrintInformation();

	CameraContext context = m_manager->GetCameraContext();

	// Proportional Controller
	Vector3 const diffInPosition	= goalState.m_position - currentState.m_position;
	Vector3 const suggestVelocity	= diffInPosition * m_controllingFactor;
	Vector3 const velocityAtTarget	= context.anchorGameObject->m_velocity;
	Vector3 const mpcVelocity		= suggestVelocity + velocityAtTarget;
	
	// Final State to return
	CameraState finalState( goalState );
	finalState.m_position	= currentState.m_position;
	finalState.m_velocity	= mpcVelocity;

	// Move according to velocity
	finalState.m_position += finalState.m_velocity * deltaSeconds;

	// Making sure that the camera is looking at the target position
	Vector3		anchorWorldPosition	= context.anchorGameObject->m_transform.GetWorldPosition();
	Matrix44	lookAtAnchorMatrix	= Matrix44::MakeLookAtView( anchorWorldPosition, finalState.m_position );
	Quaternion	cameraOrientation	= Quaternion::FromMatrix( lookAtAnchorMatrix ).GetInverse();
	finalState.m_orientation		= cameraOrientation;

	return finalState;
}

void CMC_ProportionalController::ProcessInput()
{
	// Change controlling fraction as per input
	if( g_theInput->IsKeyPressed( UP ) )
		m_controllingFactor += 3.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->IsKeyPressed( DOWN ) )
		m_controllingFactor -= 3.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->IsKeyPressed( RIGHT ) )
		m_accelerationLimitXZ += 10.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->IsKeyPressed( LEFT ) )
		m_accelerationLimitXZ -= 10.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->WasKeyJustPressed( 'M' ) )
		m_mpcEnabled = !m_mpcEnabled;
	if( g_theInput->IsKeyPressed( PAGE_DOWN ) )
		m_leadFactor -= 2.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->IsKeyPressed( PAGE_UP ) )
		m_leadFactor += 2.f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );

	// Clamp the factors
	m_controllingFactor		= (m_controllingFactor < 0.f)	 ? 0.f : m_controllingFactor;
	m_accelerationLimitXZ	= (m_accelerationLimitXZ < 0.f ) ? 0.f : m_accelerationLimitXZ;
	m_leadFactor			= (m_leadFactor < 0.f)			 ? 0.f : m_leadFactor;
}

void CMC_ProportionalController::DebugPrintInformation() const
{
	// Print it on Debug Screen
	std::string pcTypeString = Stringf( "Currently Active: %s", m_mpcEnabled ? "MPC" : "PC" );
	DebugRender2DText( 0.f, Vector2(-850.f, 380.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, pcTypeString.c_str() );
	std::string mpcEnableStr = "Press [M] to enable toggle MPC";
	DebugRender2DText( 0.f, Vector2(-850.f, 360.f), 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, mpcEnableStr.c_str() );
	std::string upDownStr = "Press [UP] [DOWN] to change the controlling factor.";
	DebugRender2DText( 0.f, Vector2(-850.f, 340.f), 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, upDownStr.c_str() );
	std::string leftRightStr = "Press [Right] [LEFT] to change the acceleration limit.";
	DebugRender2DText( 0.f, Vector2(-850.f, 320.f), 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, leftRightStr.c_str() );
	std::string trianglesStr = "Press [PG UP] [PG DOWN] to change the lead factor.";
	DebugRender2DText( 0.f, Vector2(-850.f, 300.f), 15.f, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, trianglesStr.c_str() );

	// Controlling Factor
	std::string controllingFractionStr = Stringf( "%-24s = %f", "Controlling Factor", m_controllingFactor );
	DebugRender2DText( 0.f, Vector2(-850.f, 280.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, controllingFractionStr.c_str() );

	// Acceleration Limit
	std::string accelerationLimitStr = Stringf( "%-24s = %f", "Acceleration Limit XZ", m_accelerationLimitXZ );
	DebugRender2DText( 0.f, Vector2(-850.f, 260.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, accelerationLimitStr.c_str() );

	// Lead Factor
	std::string leadFactorStr = Stringf( "%-24s = %f", "Lead Factor", m_leadFactor );
	DebugRender2DText( 0.f, Vector2(-850.f, 240.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, leadFactorStr.c_str() );
}
