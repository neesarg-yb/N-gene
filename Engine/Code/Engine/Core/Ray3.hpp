#pragma once
#include "Engine/Math/Vector3.hpp"

struct Ray3
{
public:
	Vector3 startPosition	= Vector3::ZERO;
	Vector3 direction		= Vector3::ZERO;

public:
	Ray3() { }
	Ray3( Vector3 const &startPosition, Vector3 const &direction );

public:
	Vector3 Evaluate( float t );		// Returns a point t along the ray
};