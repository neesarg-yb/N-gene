#pragma once
#include "CC_LineOfSight.hpp"

CC_LineOfSight::CC_LineOfSight( CameraManager &manager )
	: CameraConstrain( "LineOfSight", manager )
{

}

CC_LineOfSight::~CC_LineOfSight()
{

}

void CC_LineOfSight::Execute( CameraDestination &suggestedDestination )
{

}

