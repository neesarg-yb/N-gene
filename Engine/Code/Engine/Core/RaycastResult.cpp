#pragma once
#include "RaycastResult.hpp"

RaycastResult::RaycastResult( Vector3 const &startPosition )
{
	// Set values as if impact didn't happen at startPosition
	didImpact			= false;
	impactPosition		= startPosition;
	impactNormal		= Vector3::ZERO;
	fractionTravelled	= 0.f;
}

RaycastResult::RaycastResult( Vector3 const &impactPosition, Vector3 const &impactNormal, float fractionTravelled )
{
	this->didImpact			= true;
	this->impactPosition	= impactPosition;
	this->impactNormal		= impactNormal;
	this->fractionTravelled	= fractionTravelled;
}
