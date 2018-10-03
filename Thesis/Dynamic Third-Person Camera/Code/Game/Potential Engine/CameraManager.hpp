#pragma once
#include <map>
#include <vector>
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Potential Engine/GameObject.hpp"
#include "Game/Potential Engine/CameraBehaviour.hpp"
#include "Game/Potential Engine/CameraConstrain.hpp"
#include "Game/Potential Engine/CameraDestination.hpp"

typedef std::vector< CameraBehaviour* >				CameraBehaviourList;
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
	GameObject				*m_anchor = nullptr;		// Anchor GameObject

	// Camera Behaviours
	CameraBehaviourList		 m_cameraBehaviours;		// All the behaviors that can be run on Camera
	CameraBehaviour*		 m_aciveBehaviour = nullptr;
	CameraDestination		 m_lastSuggestedPoint;
	CameraDestination		 m_targetPointOnTransitionBegin;
	
	float const m_behaviourTransitionSeconds = 1.f;
	float		m_behaviourTransitionTimeRemaining = 0.f;

	// Camera Constrains
	CameraConstrainMap		m_registeredConstrains;

public:
	void Update( float deltaSeconds );
	void PreUpdate();
	void PostUpdate();

public:
	// Context
	void SetAnchor					( GameObject *anchor );
	
	// Behaviours
	int  AddNewCameraBehaviour		( CameraBehaviour *newCameraBehaviour );		// Now I'll manage this behavior, including deletion
	void DeleteCameraBehaviour		( CameraBehaviour *cameraBehaviourToDelete );	// Deletes it
	void DeleteCameraBehaviour		( std::string const &behaviourName );			// Deletes it
	void SetActiveCameraBehaviourTo	( std::string const &behaviourName );

	// Constrains
	void RegisterConstrain			( char const *name, CameraConstrain *newConstrain );
	void DeregisterConstrain		( char const *name );

private:
	int GetCameraBehaviourIndex( CameraBehaviour *cb );					// Returns -1 if couldn't fine it in the list
	int GetCameraBehaviourIndex( std::string const &behaviourName );	// Returns -1 if couldn't fine it in the list
};