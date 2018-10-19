#pragma once
#include <functional>
#include "Engine/Math/Vector3.hpp"

struct  RaycastResult;
typedef std::function< RaycastResult(Vector3 const &startPosition, Vector3 const &rayDirection, float maxDistance) >	raycast_std_func;
typedef std::function< Vector3		(Vector3 const &center, float radius) >												sphere_collision_func;

struct CameraContext
{
public:
	Vector3					playerPosition;
	raycast_std_func		raycastCallback;

	float					cameraCollisionRadius;
	sphere_collision_func	sphereCollisionCallback;

public:
	CameraContext( Vector3 const &playerPosition, raycast_std_func raycastCallback, float collisionRadius, sphere_collision_func sphereCollisionCallback );
};
