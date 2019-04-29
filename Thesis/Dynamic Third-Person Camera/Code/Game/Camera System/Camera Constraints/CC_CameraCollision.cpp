#pragma once
#include "CC_CameraCollision.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/CameraSystem/CameraContext.hpp"

CC_CameraCollision::CC_CameraCollision( char const *name, CameraManager &manager, uint8_t priority )
	: CameraConstraint( name, manager, priority )
{

}

CC_CameraCollision::~CC_CameraCollision()
{

}

void CC_CameraCollision::Execute( CameraState &suggestedCameraState )
{
	PROFILE_SCOPE_FUNCTION();

	CameraContext context	 = m_manager.GetCameraContext();
	bool		  didCollide = false;
	Vector3		  newCenter	 = context.sphereCollisionCallback( suggestedCameraState.m_position, context.cameraCollisionRadius, didCollide );
	
	suggestedCameraState.m_position = newCenter;
}
