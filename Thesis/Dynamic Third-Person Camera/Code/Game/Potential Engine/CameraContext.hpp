#pragma once
#include "Engine/Math/Vector3.hpp"

struct RaycastResult;
typedef RaycastResult (*raycast_cb) ( Vector3 const &startPosition, Vector3 const &rayDirection, float maxDistance );

struct CameraContext
{
public:
	Vector3		playerPosition;
	raycast_cb	raycastCallback;
};