#pragma once
#include "CC_CameraCollision.hpp"
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
	CameraContext context	= m_manager.GetCameraContext();
	Vector3		  newCenter	= context.sphereCollisionCallback( suggestedCameraState.m_position, context.cameraCollisionRadius );
	
	suggestedCameraState.m_position = newCenter;
}