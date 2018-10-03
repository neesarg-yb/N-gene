#pragma once
#include "Game/Potential Engine/CameraConstrain.hpp"

class CC_LineOfSight : public CameraConstrain
{
public:
	 CC_LineOfSight( CameraManager &manager );
	~CC_LineOfSight();

public:
	void Execute( CameraDestination &suggestedDestination );

};