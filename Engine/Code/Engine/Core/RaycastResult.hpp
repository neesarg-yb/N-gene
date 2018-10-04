#pragma once
#include "Engine/Math/Vector3.hpp"

struct RaycastResult
{
public:
	bool	didImpact;
	Vector3	impactPosition;
	float	fractionTravelled;

public:
	RaycastResult( Vector3 startPosition );
};