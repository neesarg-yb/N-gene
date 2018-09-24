#pragma once
#include <vector>
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Potential Engine/GameObject.hpp"
#include "Game/Potential Engine/CameraBehaviour.hpp"
#include "Game/Potential Engine/CameraTargetPoint.hpp"

typedef std::vector< CameraBehaviour* > CameraBehaviourList;

class CameraManager
{
public:
	 CameraManager( Camera &camera, InputSystem &inputSystem );
	~CameraManager();

private:
	InputSystem				&m_inputSystem;				// Current input system
	Camera					&m_camera;					// Possessed Camera
	GameObject				*m_anchor = nullptr;		// Anchor GameObject
	CameraBehaviourList		 m_cameraBehaviours;		// All the behaviors that can be run on Camera
	CameraBehaviour*		 m_aciveBehaviour = nullptr;

public:
	void Update( float deltaSeconds );
	void PreUpdate();
	void PostUpdate();

public:
	void SetAnchor					( GameObject *anchor );
	void AddCameraBehaviour			( CameraBehaviour *newCameraBehaviour );
	void RemoveCameraBehaviour		( CameraBehaviour *cameraBehaviourToRemove );
	void RemoveCameraBehaviour		( std::string const &behaviourName );
	void SetActiveCameraBehaviourTo	( std::string const &behaviourName );

private:
	int GetCameraBehaviourIndex( CameraBehaviour *cb );					// Returns -1 if couldn't fine it in the list
	int GetCameraBehaviourIndex( std::string const &behaviourName );		// Returns -1 if couldn't fine it in the list
};