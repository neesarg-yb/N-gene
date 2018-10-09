#pragma once
#include "Game/Potential Engine/CameraConstrain.hpp"

class CC_LineOfSight : public CameraConstrain
{
public:
	 CC_LineOfSight( CameraManager &manager, const char *name );
	~CC_LineOfSight();

public:
	void Execute( CameraState &suggestedDestination );

};