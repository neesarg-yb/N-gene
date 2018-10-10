#pragma once
#include "Game/Potential Engine/CameraConstrain.hpp"

class CC_LineOfSight : public CameraConstrain
{
public:
	 CC_LineOfSight( char const *name, CameraManager &manager, uint8_t priority );
	~CC_LineOfSight();

public:
	void Execute( CameraState &suggestedDestination );

};