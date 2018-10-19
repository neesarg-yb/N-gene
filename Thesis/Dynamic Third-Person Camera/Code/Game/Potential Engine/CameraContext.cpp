#pragma once
#include "CameraContext.hpp"

CameraContext::CameraContext( GameObject const *anchor, raycast_std_func raycastCallback, float collisionRadius, sphere_collision_func sphereCollisionCallback )
{
	this->anchorGameObject			= anchor;
	this->raycastCallback			= raycastCallback;
	this->cameraCollisionRadius		= collisionRadius;
	this->sphereCollisionCallback	= sphereCollisionCallback;
}
