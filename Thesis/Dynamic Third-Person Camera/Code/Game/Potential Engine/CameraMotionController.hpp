#pragma once
#include "Game/Potential Engine/CameraState.hpp"

class CameraManager;

class CameraMotionController
{
public:
			 CameraMotionController( char const *name, CameraManager const *manager );
	virtual	~CameraMotionController();

public:
	std::string const	 m_name		= "NAME NOT ASSIGNED!";
	CameraManager const	*m_manager	= nullptr;

public:
	virtual CameraState MoveCamera( CameraState const &currentState, CameraState const &goalState, float deltaSeconds );
};
