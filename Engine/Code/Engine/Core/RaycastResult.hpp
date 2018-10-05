#pragma once
#include "Engine/Math/Vector3.hpp"

struct RaycastResult
{
public:
	bool	didImpact;
	Vector3	impactPosition;
	Vector3 impactNormal;
	float	fractionTravelled;

public:
	RaycastResult( Vector3 const &startPosition );																// Defaults as if (didImpact = FALSE)
	RaycastResult( Vector3 const &impactPosition, Vector3 const &impactNormal, float fractionTravelled );		// didImpact = TRUE
};