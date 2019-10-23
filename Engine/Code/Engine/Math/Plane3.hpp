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
	Vector3 normal	= Vector3::ZERO;		// invalid value
	float	d		= 0.f;					// distance from origin

public:
	float GetDistanceFromPoint( Vector3 const &point ) const;	// Returns: POSITIVE if the point is on front side of the normal; NEGATIVE if point is on backside; ZERO if on plane
	bool  IsValid() const;										// Returns false if normal is ZERO Vector

	Vector3 VectorProjection( Vector3 const &vector ) const;
};

Vector3 ProjectVectorOnPlane( Vector3 const &vector, Vector3 const &normal );

