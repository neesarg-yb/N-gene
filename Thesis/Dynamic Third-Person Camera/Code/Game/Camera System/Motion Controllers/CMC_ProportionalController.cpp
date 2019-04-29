#pragma once
#include "CMC_ProportionalController.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/GameCommon.hpp"

Vector3 g_debugCameraMPCVelThisFrame;		// Only here for debug purpose

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

	// Look ahead according to player velocity
	UpdateLeadOffset( *context.anchorGameObject, deltaSeconds );

	// Proportional Controller
	Vector3 const leadOffsetFromGoal	= Vector3( m_leadOffsetFromGoal.x, 0.f, m_leadOffsetFromGoal.y );
	Vector3 const goalPosWithOffset		= goalState.m_position + leadOffsetFromGoal;
	Vector3 const diffInPosition		= goalPosWithOffset - currentState.m_position;
	Vector3 const suggestVelocity		= diffInPosition * m_controllingFactor;
	Vector3 const velocityAtTarget		= context.anchorGameObject->m_velocity;
	Vector3 const mpcVelocity			= suggestVelocity + velocityAtTarget;		// Adding the velocityAtTarget might make the camera go into geometry

	// Final State to return
	CameraState finalState( goalState );
	finalState.m_position = currentState.m_position;
	finalState.m_velocity = mpcVelocity;
	g_debugCameraMPCVelThisFrame = mpcVelocity;

	// Move according to velocity
	finalState.m_position += finalState.m_velocity * deltaSeconds;

	// Make sure not in the collision
	if( m_finalCollisionCheckEnabled )
		FinalCollisionCheck( currentState.m_position, goalState.m_position, finalState, deltaSeconds );

	// DebugRenderPoint( 10.f, 0.1f, finalState.m_position, RGBA_BLUE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_USE_DEPTH );

	// Making sure that the camera is looking at the target position
	Vector3		anchorWorldPosition	= context.anchorGameObject->m_transform.GetWorldPosition();
	Matrix44	lookAtAnchorMatrix	= Matrix44::MakeLookAtView( anchorWorldPosition + leadOffsetFromGoal, finalState.m_position );
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
		m_leadInterpolationFactor -= 0.1f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->IsKeyPressed( PAGE_UP ) )
		m_leadInterpolationFactor += 0.1f * (float)( GetMasterClock()->GetFrameDeltaSeconds() );
	if( g_theInput->WasKeyJustPressed( 'Y' ) )
		m_finalCollisionCheckEnabled = !m_finalCollisionCheckEnabled;

	// Clamp the factors
	m_controllingFactor			= (m_controllingFactor < 0.f)	 ? 0.f : m_controllingFactor;
	m_accelerationLimitXZ		= (m_accelerationLimitXZ < 0.f ) ? 0.f : m_accelerationLimitXZ;
	m_leadInterpolationFactor	= ClampFloat01( m_leadInterpolationFactor );
}

Vector2 CMC_ProportionalController::UpdateLeadOffset( GameObject const &player, float deltaSeconds )
{
	Vector2	const playerCurrentVelocity	= Vector2( player.m_velocity.x, player.m_velocity.z );
	float	const playerCurrentSpeed	= playerCurrentVelocity.GetLength();

	// Get XZ Direction of player's velocity
	Vector2 playerVelocityDir = playerCurrentVelocity.GetNormalized();

	// Set the desired offset
	float	playerSpeedFraction		= ClampFloat01( playerCurrentSpeed / PLAYER_MAX_SPEED );
	Vector2 desiredOffsetFromGoal	= playerVelocityDir * playerSpeedFraction;

	// Interpolate towards it..
	m_leadOffsetFromGoal = Interpolate( m_leadOffsetFromGoal, desiredOffsetFromGoal, m_leadInterpolationFactor * deltaSeconds );

	// Debug Render the offset relative to player
	// DebugRenderPoint( 0.f, 1.f, player.m_transform.GetWorldPosition() + Vector3( m_leadOffsetFromGoal.x, 0.f, m_leadOffsetFromGoal.y ), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );

	return m_leadOffsetFromGoal;
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
	std::string controllingFractionStr = Stringf( "%-26s = %f", "Controlling Factor", m_controllingFactor );
	DebugRender2DText( 0.f, Vector2(-850.f, 280.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, controllingFractionStr.c_str() );

	// Acceleration Limit
	std::string accelerationLimitStr = Stringf( "%-26s = %f", "Acceleration Limit XZ", m_accelerationLimitXZ );
	DebugRender2DText( 0.f, Vector2(-850.f, 260.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, accelerationLimitStr.c_str() );

	// Lead Factor
	std::string leadFactorStr = Stringf( "%-26s = %f", "Lead Interpolation Factor", m_leadInterpolationFactor );
	DebugRender2DText( 0.f, Vector2(-850.f, 240.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, leadFactorStr.c_str() );

	// Final Collision Check
	Vector2 finalCollisionCheckTxtStrPos = Vector2( 300.f, 180.f );
	DebugRender2DText( 0.f, finalCollisionCheckTxtStrPos, 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, Stringf( "[Y] Final Collision Check: \"%s\"", m_finalCollisionCheckEnabled ? "ENABLED" : "disabled" ) );
}

void CMC_ProportionalController::FinalCollisionCheck( Vector3 const &currentPosition, Vector3 const &safeDestination, CameraState &goalState, float deltaSeconds )
{
	CameraContext context = m_manager->GetCameraContext();
	
	// DEBUG-RENDER
	bool didCurrentCollide = false;
	Vector3	correctedCurrentPosition = context.sphereCollisionCallback( currentPosition, context.cameraCollisionRadius, didCurrentCollide );
	Vector2 currentCollideTxtPos = Vector2( 400.f, 200.f );
	Rgba currenCollideColor = didCurrentCollide ? RGBA_RED_COLOR : RGBA_GREEN_COLOR;
	DebugRender2DText( 0.f, currentCollideTxtPos, 15.f, currenCollideColor, currenCollideColor, Stringf("Curr Pos: %s", didCurrentCollide ? "COLLIDED" : "No Collision" ) );
	DebugRenderPoint( 0.f, 0.2f, currentPosition, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );

	bool didSafeDestCollide = false;
	Vector3	correctedSafePosition = context.sphereCollisionCallback( safeDestination, context.cameraCollisionRadius, didSafeDestCollide );
	Vector2 safeDestTxtPos = currentCollideTxtPos + Vector2( 0.f, 20.f );
	Rgba safeDestCollideColor = didSafeDestCollide ? RGBA_RED_COLOR : RGBA_GREEN_COLOR;
	DebugRender2DText( 0.f, safeDestTxtPos, 15.f, safeDestCollideColor, safeDestCollideColor, Stringf("Safe Pos: %s", didSafeDestCollide ? "COLLIDED" : "No Collision") );
	DebugRenderPoint( 0.f, 0.2f, safeDestination, RGBA_YELLOW_COLOR, RGBA_YELLOW_COLOR, DEBUG_RENDER_XRAY );

	bool didGoalCollide = false;
	Vector3	correctedGoalPosition = context.sphereCollisionCallback( goalState.m_position, context.cameraCollisionRadius, didGoalCollide );
	Vector2 goalCollideTxtPos = safeDestTxtPos + Vector2( 0.f, 20.f );
	Rgba goalCollideColor = didGoalCollide ? RGBA_RED_COLOR : RGBA_GREEN_COLOR;
	DebugRender2DText( 0.f, goalCollideTxtPos, 15.f, goalCollideColor, goalCollideColor, Stringf("Goal Pos: %s", didGoalCollide ? "COLLIDED" : "No Collision" ) );
	DebugRenderPoint( 0.f, 0.2f, goalState.m_position, goalCollideColor, goalCollideColor, DEBUG_RENDER_XRAY );

 	if( didGoalCollide == false )
 	{
 		// Not colliding, safe to move to the goal
		DebugRenderVector( 0.f, currentPosition, g_debugCameraMPCVelThisFrame * deltaSeconds, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );

		// i.e. Goal state stays unchanged!
 	}
 	else
 	{
 		// Colliding, do not move to the goal position
		//          instead, move to the safe destination
		
		// Proportional Controller
		Vector3 const diffInPosition	= safeDestination - currentPosition;
		Vector3 const suggestVelocity	= diffInPosition * m_controllingFactor;
		DebugRenderVector( 0.f, currentPosition, suggestVelocity * deltaSeconds, RGBA_RED_COLOR, RGBA_RED_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );

		Vector3 pcPosition = currentPosition + (suggestVelocity * deltaSeconds);
		
		goalState.m_position = pcPosition;
		goalState.m_velocity = suggestVelocity;
 	}
}
