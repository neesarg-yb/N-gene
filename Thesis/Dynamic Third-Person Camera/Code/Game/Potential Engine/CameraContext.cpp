#pragma once
#include "CameraContext.hpp"

CameraContext::CameraContext( Vector3 const &playerPosition, raycast_std_func raycastCallback, float collisionRadius, sphere_collision_func sphereCollisionCallback )
{
	this->playerPosition			= playerPosition;
	this->raycastCallback			= raycastCallback;
	this->cameraCollisionRadius		= collisionRadius;
	this->sphereCollisionCallback	= sphereCollisionCallback;
}
