#pragma once
#include "Engine/Math/Vector3.hpp"

struct ContactPoint
{
public:
	Vector3 position = Vector3::ZERO;
	Vector3 normal	 = Vector3::ZERO;

public:
	ContactPoint() { }
	ContactPoint( Vector3 const &position, Vector3 const &normal );
};