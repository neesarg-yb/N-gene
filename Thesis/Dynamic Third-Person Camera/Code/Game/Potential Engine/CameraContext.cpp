#pragma once
#include "CameraContext.hpp"

CameraContext::CameraContext( GameObject const *anchor, raycast_std_func raycastCallback, float collisionRadius, sphere_collision_func sphereCollisionCallback, CameraState const &stateLastFrame )
{
	this->anchorGameObject			= anchor;
	this->raycastCallback			= raycastCallback;
	this->cameraCollisionRadius		= collisionRadius;
	this->sphereCollisionCallback	= sphereCollisionCallback;
	this->cameraStateLastFrame		= stateLastFrame;
}
