#pragma once
#include "Quaternions.hpp"
#include "Engine/Math/MathUtil.hpp"

Quaternions::Quaternions()
{
	// Identity quaternion, by default values
}

Quaternions::Quaternions( Vector3 const &axis, float rotationDegrees )
{
	float halfRotationRadians = DegreeToRadian( rotationDegrees ) * 0.5f;

	r	= cosf( halfRotationRadians );
	i.x	= axis.x * sinf( halfRotationRadians );
	i.y	= axis.y * sinf( halfRotationRadians );
	i.z	= axis.z * sinf( halfRotationRadians );
}

Quaternions::~Quaternions()
{

}

Quaternions Quaternions::GetInvert() const
{
	Quaternions qInvert;

	qInvert.r = r;
	qInvert.i = i * -1.f;

	return qInvert;
}

Quaternions Quaternions::Multiply( Quaternions const b ) const
{
	Quaternions ab; // = a.Multiply( b )

	ab.r = ( b.r * r ) - Vector3::DotProduct( i, b.i );
	ab.i = ( i * b.r ) + ( b.i * r ) + Vector3::CrossProduct( i, b.i );

	return ab;
}

Vector3 Quaternions::RotatePoint( Vector3 const point ) const
{
	// Derived equation comes from: Rotation through multiplication of two quaternions
	//
	// Quaternions p;
	// p.r = 0.f;
	// p.i = point;
	//
	// return ( q * p * q.GetInvert() ).i;				// Order of multiplication goes left to right -->

	Vector3 iPointCross = Vector3::CrossProduct(i, point);
	Vector3 pRotated	= point 
						+ ( iPointCross * 2.f * r ) 
						+ ( Vector3::CrossProduct(i, iPointCross) * 2.f );

	return pRotated;
}

