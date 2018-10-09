#pragma once
#include "Game/Potential Engine/CameraState.hpp"

class CameraManager;

class CameraConstrain
{
public:
	 CameraConstrain( char const *name, CameraManager &manager );
	~CameraConstrain();

protected:
	std::string const	&m_name		= "NAME NOT ASSIGNED!";
	CameraManager		&m_manager;

public:
	virtual void Execute( CameraState &suggestedCameraState ) = 0;	// Applies the constrain on suggestedDestination
};
