#pragma once
#include <string>
#include "Engine/Core/Tags.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Potential Engine/GameObject.hpp"
#include "Game/Potential Engine/CameraState.hpp"

class CameraBehaviour
{
public:
			 CameraBehaviour( std::string const &behaviourName );
	virtual ~CameraBehaviour();

public:
	std::string	const		 m_name			= "NAME NOT ASSIGNED!";
	Camera		const		*m_camera		= nullptr;
	GameObject	const		*m_anchor		= nullptr;
	InputSystem				*m_inputSystem	= nullptr;
	Tags					 m_constrains;	// Which constrains to apply by Camera Manager

public:
	virtual void			PreUpdate () = 0;
	virtual void			PostUpdate() = 0;
	virtual CameraState		Update( float deltaSeconds ) = 0;

public:
	void SetCameraAnchorAndInputSystemTo( Camera *camera, GameObject *anchor, InputSystem *input );
};