#pragma once
#include "Engine/Math/Vector3.hpp"

class Plane3
{
public:
	 Plane3();								// Invalid Plane
	 Plane3( Vector3 normal, float d );
	 Plane3( Vector3 normal, Vector3 pointOnPlane );
	~Plane3();

public:
	// Equation:
	//			normal * (x, y, z) = d
	//		=>	ax + by + cz = d
	Vector3 normal	= Vector3::ZERO;		// Normal = (a, b, c)
	float	d		= 0.f;					// Distance from origin

public:
	float GetDistanceFromPoint( Vector3 const &point );
	bool  IsValid() const;					// Returns false if normal is ZERO Vector

	Vector3 VectorProjection( Vector3 const &vector );
};

Vector3 ProjectVectorOnPlane( Vector3 const &vector, Vector3 const &normal );

