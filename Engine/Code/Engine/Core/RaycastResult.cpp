#pragma once
#include "RaycastResult.hpp"

RaycastResult::RaycastResult( Vector3 const &endPosition )
{
	// Set values as if impact didn't happen at startPosition
	didImpact			= false;
	impactPosition		= endPosition;
	impactNormal		= Vector3::ZERO;
	fractionTravelled	= 1.f;
}

RaycastResult::RaycastResult( Vector3 const &impactPosition, Vector3 const &impactNormal, float fractionTravelled )
{
	this->didImpact			= true;
	this->impactPosition	= impactPosition;
	this->impactNormal		= impactNormal;
	this->fractionTravelled	= fractionTravelled;
}
