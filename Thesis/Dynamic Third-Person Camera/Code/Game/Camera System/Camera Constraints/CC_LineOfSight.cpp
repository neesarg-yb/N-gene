#pragma once
#include "CC_LineOfSight.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"

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
	float	raycastDistance	= towardsCamera.NormalizeAndGetLength();

	RaycastResult hitResult = context.raycastCallback( playerPosition, towardsCamera, raycastDistance );
	if( hitResult.didImpact )
		suggestedCameraState.m_position = hitResult.impactPosition - (towardsCamera * context.cameraCollisionRadius);
}

