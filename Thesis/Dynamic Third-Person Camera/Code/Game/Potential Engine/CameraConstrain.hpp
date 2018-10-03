#pragma once
#include "Game/Potential Engine/CameraDestination.hpp"

class CameraManager;

class CameraConstrain
{
public:
	 CameraConstrain( char const *name, CameraManager &manager );
	~CameraConstrain();

private:
	std::string const	&m_name		= "NAME NOT ASSIGNED!";
	CameraManager		&m_manager;

public:
	virtual void Execute( CameraDestination &suggestedDestination ) = 0;
};
