#pragma once
#include "CC_LineOfSight.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/GameCommon.hpp"

CC_LineOfSight::CC_LineOfSight( char const *name, CameraManager &manager, uint8_t priority )
	: CameraConstraint( name, manager, priority )
{
	
}

CC_LineOfSight::~CC_LineOfSight()
{

}

void CC_LineOfSight::Execute( CameraState &suggestedCameraState )
{
	PROFILE_SCOPE_FUNCTION();
	
	// Get context
	CameraContext context = m_manager.GetCameraContext();

	// Player's Position
	Vector3	playerPosition	= context.anchorGameObject->m_transform.GetWorldPosition();
	Vector3	cameraPosition	= suggestedCameraState.m_position;
	Vector3	towardsCamera	= cameraPosition - playerPosition;
	float	raycastDistance	= towardsCamera.NormalizeAndGetLength() + m_radiusReduction;

	RaycastResult hitResult = context.raycastCallback( playerPosition, towardsCamera, raycastDistance );
	if( hitResult.didImpact )
		suggestedCameraState.m_position = hitResult.impactPosition - (towardsCamera * m_radiusReduction);

	DebugRenderRaycast( 0.f, playerPosition, hitResult, 1.f, RGBA_PURPLE_COLOR, RGBA_WHITE_COLOR, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_XRAY );
}

void CC_LineOfSight::ProcessDebugInput()
{
	float changeRate = 1.f;
	float deltaSeconds = (float) GetMasterClock()->GetFrameDeltaSeconds();

	bool increase = g_theInput->IsKeyPressed( 'M' );
	bool decrease = g_theInput->IsKeyPressed( 'N' );

	float deltaRadiusSize = 0.f;
	deltaRadiusSize += increase ? (changeRate * deltaSeconds) : 0.f;
	deltaRadiusSize -= decrease ? (changeRate * deltaSeconds) : 0.f;

	m_radiusReduction += deltaRadiusSize;
	DebugRender2DText( 0.f, Vector2( -50.f, -50.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, Stringf( "LoS RadiusSize = %.2f", m_radiusReduction ) );
}

