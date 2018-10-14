#pragma once
#include "Game/Potential Engine/CameraState.hpp"

class CameraManager;

class CameraConstrain
{
public:
			 CameraConstrain( char const *name, CameraManager &manager, uint8_t priority );
	virtual ~CameraConstrain();

public:
	uint8_t		const	 m_priority; // (0 to 255) : Higher the value, higher the priority
	std::string const	 m_name		= "NAME NOT ASSIGNED!";

protected:
	CameraManager		&m_manager;

public:
	virtual void Execute( CameraState &suggestedCameraState ) = 0;	// Applies the constrain on suggestedDestination

public:
	// Operators compares the priority
	bool operator < ( CameraConstrain const& b ) const;
	bool operator > ( CameraConstrain const& b ) const;
};
