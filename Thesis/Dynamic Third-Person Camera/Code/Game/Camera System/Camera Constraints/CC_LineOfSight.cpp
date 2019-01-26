#pragma once
#include "CC_LineOfSight.hpp"
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
	// Get context
	CameraContext context = m_manager.GetCameraContext();

	// Player's Position
	Vector3 playerPosition = context.anchorGameObject->m_transform.GetWorldPosition();

	// Move the CameraState until we get clear line of sight
	bool hasClearLineOfSight = false;
	while( hasClearLineOfSight == false )
	{
		// Camera's Position
		Vector3 cameraPosition = suggestedCameraState.m_position;

		// Raycast hit result
		Vector3	towardsPlayer	= ( playerPosition - cameraPosition );
		float	raycastDistance	= towardsPlayer.NormalizeAndGetLength();
		RaycastResult hitResult = context.raycastCallback( cameraPosition, towardsPlayer, raycastDistance );

		// Change camera destination to hit-result position, a little bit towards the player
		if( hitResult.didImpact == false )
		{
			hasClearLineOfSight = true;
			break;
		}
		else
		{
			// Move the camera at hit-point
			suggestedCameraState.m_position = hitResult.impactPosition;
			hasClearLineOfSight = false;
			continue;
		}
	}
}

