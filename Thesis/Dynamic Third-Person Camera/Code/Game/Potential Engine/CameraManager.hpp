#pragma once
#include <queue>
#include <vector>
#include "Engine/Core/Tags.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Potential Engine/GameObject.hpp"
#include "Game/Potential Engine/CameraBehaviour.hpp"
#include "Game/Potential Engine/CameraConstrain.hpp"
#include "Game/Potential Engine/CameraState.hpp"
#include "Game/Potential Engine/CameraContext.hpp"


struct CustomCameraConstarinCompare
{
	// Such that top element is the lowest priority, and bottom is the highest priority
	bool operator() ( CameraConstrain const *lhs, CameraConstrain const *rhs ) const
	{
		return (*lhs > *rhs);
	}
};

typedef std::vector< CameraBehaviour* >																CameraBehaviourList;
typedef std::vector< CameraConstrain* >																CameraConstrainList;
typedef std::priority_queue< CameraConstrain*, CameraConstrainList, CustomCameraConstarinCompare >	CameraConstrainPriorityQueue;		// Top: lowest priority & Bottom: highest priority

class CameraManager
{
public:
	 CameraManager( Camera &camera, InputSystem &inputSystem );
	~CameraManager();

private:
	// Camera Context
	InputSystem				&m_inputSystem;				// Current input system
	Camera					&m_camera;					// Possessed Camera
	float					 m_cameraRadius = 1.f;		// It should be based on Camera's near-plane
	GameObject				*m_anchor		= nullptr;	// Anchor GameObject
	raycast_std_func		 m_raycastCB	= nullptr;	// Raycast function, provided from Game Side
	sphere_collision_func	 m_collisionCB	= nullptr;	// Sphere Collision function, provided from Game Side

	// Camera Behaviours
	CameraBehaviourList		 m_cameraBehaviours;		// All the behaviors that can be run on Camera
	CameraBehaviour			*m_aciveBehaviour = nullptr;
	CameraState				 m_lastSuggestedState;
	CameraState				 m_stateOnTransitionBegin;
	
	float const m_behaviourTransitionSeconds = 1.f;
	float		m_behaviourTransitionTimeRemaining = 0.f;

	// Camera Constrains
	bool							m_constrainsEnabled = true;
	CameraConstrainList				m_registeredConstrains;		// Stored without any sorting by priority
	CameraConstrainPriorityQueue	m_activeConstrains;			// Lowest priority at top, highest at bottom

public:
	void Update( float deltaSeconds );
	void PreUpdate();
	void PostUpdate();

public:
	// Context
	void			SetAnchor( GameObject *anchor );
	void			SetRaycastCallback( raycast_std_func raycastFunction );
	void			SetSphereCollisionCallback( sphere_collision_func collisionFunction );
	float			GetCameraRadius() const;
	CameraContext	GetCameraContext() const;
	
	// Behaviours
	int  AddNewCameraBehaviour		( CameraBehaviour *newCameraBehaviour );		// Now I'll manage this behavior, including deletion
	void DeleteCameraBehaviour		( CameraBehaviour *cameraBehaviourToDelete );	// Deletes it
	void DeleteCameraBehaviour		( std::string const &behaviourName );			// Deletes it
	void SetActiveCameraBehaviourTo	( std::string const &behaviourName );

	// Constrains
	void RegisterConstrain	( CameraConstrain* newConstrain );
	void DeregisterConstrain( char const *name );
	void EnableConstrains	( bool enable = true );

private:
	void ResetActivateConstrainsFromTags ( Tags const &constrainsToActivate );

private:
	int GetCameraBehaviourIndex( CameraBehaviour *cb );					// Returns -1 if couldn't find it in the list
	int GetCameraBehaviourIndex( std::string const &behaviourName );	// Returns -1 if couldn't find it in the list
	int GetCameraConstrainIndex( std::string const &constrainName );	// Returns -1 if couldn't find it in the list
};
