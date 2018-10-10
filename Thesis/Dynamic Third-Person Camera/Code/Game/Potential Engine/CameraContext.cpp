#pragma once
#include "CameraContext.hpp"

CameraContext::CameraContext( Vector3 const &playerPosition, raycast_std_func raycastCallback )
{
	this->playerPosition	= playerPosition;
	this->raycastCallback	= raycastCallback;
}
