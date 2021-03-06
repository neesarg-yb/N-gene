#pragma once
#include <string>
#include "Engine/Core/Tags.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/CameraSystem/CameraState.hpp"

class CameraManager;

class CameraBehaviour
{
public:
			 CameraBehaviour( std::string const &behaviourName, CameraManager *manager );
	virtual ~CameraBehaviour();

public:
	std::string	const		 m_name					= "NAME NOT ASSIGNED!";
	Camera		const		*m_camera				= nullptr;
	GameObject	const		*m_anchor				= nullptr;
	InputSystem				*m_inputSystem			= nullptr;
	std::string				 m_motionControllerName	= "";
	Tags					 m_constraints;			// Which constraints to apply by Camera Manager

protected:
	CameraManager			*m_manager		= nullptr;

public:
	virtual void			PreUpdate () = 0;
	virtual void			PostUpdate() = 0;
	virtual CameraState		Update( float deltaSeconds, CameraState const &currentState ) = 0;

public:
	void SetCameraAnchorAndInputSystemTo( Camera *camera, GameObject *anchor, InputSystem *input );
};