#pragma once
#include "Engine/CameraSystem/CameraState.hpp"

class CameraManager;

class CameraConstraint
{
public:
			 CameraConstraint( char const *name, CameraManager &manager, uint8_t priority );
	virtual ~CameraConstraint();

public:
	uint8_t		const	 m_priority; // (0 to 255) : Higher the value, higher the priority
	std::string const	 m_name		= "NAME NOT ASSIGNED!";

protected:
	CameraManager		&m_manager;

public:
	virtual void Execute( CameraState &suggestedCameraState ) = 0;	// Applies the constraint on suggestedDestination

public:
	// Operators compares the priority
	bool operator < ( CameraConstraint const& b ) const;
	bool operator > ( CameraConstraint const& b ) const;
};
