#pragma once
#include "RaycastResult.hpp"

RaycastResult::RaycastResult( Vector3 startPosition )
{
	// Set values as if impact didn't happen at startPosition
	didImpact			= false;
	impactPosition		= startPosition;
	fractionTravelled	= 0.f;
}