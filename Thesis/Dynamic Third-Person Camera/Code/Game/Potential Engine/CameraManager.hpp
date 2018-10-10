#pragma once
#include <map>
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

typedef std::vector< CameraBehaviour* >				CameraBehaviourList;
typedef std::vector< CameraConstrain* >				CameraConstrainList;
typedef std::map< std::string, CameraConstrain* >	CameraConstrainMap;

class CameraManager
{
public:
	 CameraManager( Camera &camera, InputSystem &inputSystem );
	~CameraManager();

private:
	// Camera Context
	InputSystem				&m_inputSystem;				// Current input system
	Camera					&m_camera;					// Possessed Camera
	GameObject				*m_anchor	 = nullptr;		// Anchor GameObject
	raycast_std_func		 m_raycastCB = nullptr;		// Raycast function provided from Game Side

	// Camera Behaviours
	CameraBehaviourList		 m_cameraBehaviours;		// All the behaviors that can be run on Camera
	CameraBehaviour			*m_aciveBehaviour = nullptr;
	CameraState				 m_lastSuggestedState;
	CameraState				 m_stateOnTransitionBegin;
	
	float const m_behaviourTransitionSeconds = 1.f;
	float		m_behaviourTransitionTimeRemaining = 0.f;

	// Camera Constrains
	bool					m_constrainsEnabled = true;
	CameraConstrainMap		m_registeredConstrains;
	CameraConstrainList		m_activeConstrains;

public:
	void Update( float deltaSeconds );
	void PreUpdate();
	void PostUpdate();

public:
	// Context
	void			SetAnchor( GameObject *anchor );
	void			SetRaycastCallback( raycast_std_func raycastFunction );
	CameraContext	GetCameraContext() const;
	
	// Behaviours
	int  AddNewCameraBehaviour		( CameraBehaviour *newCameraBehaviour );		// Now I'll manage this behavior, including deletion
	void DeleteCameraBehaviour		( CameraBehaviour *cameraBehaviourToDelete );	// Deletes it
	void DeleteCameraBehaviour		( std::string const &behaviourName );			// Deletes it
	void SetActiveCameraBehaviourTo	( std::string const &behaviourName );

	// Constrains
	void RegisterConstrain	( char const *name, CameraConstrain *newConstrain );
	void DeregisterConstrain( char const *name );
	void EnableConstrains	( bool enable = true );

private:
	void ResetActivateConstrainsFromTags ( Tags const &constrainsToActivate );

private:
	int GetCameraBehaviourIndex( CameraBehaviour *cb );					// Returns -1 if couldn't fine it in the list
	int GetCameraBehaviourIndex( std::string const &behaviourName );	// Returns -1 if couldn't fine it in the list
};
