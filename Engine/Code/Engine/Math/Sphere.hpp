#pragma once
#include "Engine/Math/Vector3.hpp"

class Sphere
{
public:
	 Sphere(); // Sets as a point with center at ZERO position
	 Sphere( Vector3 const &centerPos, float radiusLength );
	~Sphere();

public:
	Vector3	center	= Vector3::ZERO;
	float	radius	= 0.f;
};