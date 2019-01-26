#pragma once
#include "Engine/CameraSystem/CameraConstraint.hpp"

class CC_LineOfSight : public CameraConstraint
{
public:
	 CC_LineOfSight( char const *name, CameraManager &manager, uint8_t priority );
	~CC_LineOfSight();

public:
	void Execute( CameraState &suggestedDestination );

};