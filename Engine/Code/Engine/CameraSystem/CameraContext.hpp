#pragma once
#include <functional>
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/CameraSystem/CameraState.hpp"

struct  RaycastResult;
typedef std::function< RaycastResult(Vector3 const &startPosition, Vector3 const &rayDirection, float maxDistance) >	raycast_std_func;
typedef std::function< Vector3		(Vector3 const &center, float radius, bool &didCollide_out) >						sphere_collision_func;

struct CameraContext
{
public:
	GameObject const		*anchorGameObject;
	raycast_std_func		 raycastCallback;

	float					 cameraCollisionRadius;
	sphere_collision_func	 sphereCollisionCallback;

	CameraState				 cameraStateLastFrame;

public:
	CameraContext( GameObject const *anchor, raycast_std_func raycastCallback, float collisionRadius, sphere_collision_func sphereCollisionCallback, CameraState const &stateLastFrame );
};
