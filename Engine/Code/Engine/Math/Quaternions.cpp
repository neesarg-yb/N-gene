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

void Quaternions::Normalize()
{
	float const lengthSquared = GetMagnitudeSquared();

	// If already normalized
	if( lengthSquared == 1.f )
		return;

	if( r < 0.9999f )
	{
		float oneBylength = 1.f / sqrtf( lengthSquared );
		
		r = r * oneBylength;
		i = i * oneBylength;
	}
	else
	{
		r = 1.f;
		i = Vector3::ZERO;
	}
}

float Quaternions::GetMagnitude() const
{
	return sqrtf( GetMagnitudeSquared() );
}

float Quaternions::GetMagnitudeSquared() const
{
	return DotProduct( *this, *this );
}

bool Quaternions::IsUnitQuaternion() const
{
	return ( 1.f == GetMagnitudeSquared() );
}

Quaternions Quaternions::GetInverse() const
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

Vector3 Quaternions::GetAsEuler() const
{
	Matrix44 mat44 = GetAsMatrix44();

	return mat44.GetEulerRotation();
}

Matrix44 Quaternions::GetAsMatrix44() const
{
	Vector3 iBasis = Vector3( 1.f -	(2.f * i.y * i.y) - (2.f * i.z * i.z),
									(2.f * i.x * i.y) + (2.f * i.z * r),
									(2.f * i.x * i.z) - (2.f * i.y * r) );
	Vector3 jBasis = Vector3(		(2.f * i.x * i.y) - (2.f * i.z * r),
							 1.f -	(2.f * i.x * i.x) - (2.f * i.z * i.z),
									(2.f * i.y * i.z) + (2.f * i.x * r) );
	Vector3 kBasis = Vector3(		(2.f * i.x * i.z) + (2.f * i.y * r),
									(2.f * i.y * i.z) - (2.f * i.x * r),
							 1.f -	(2.f * i.x * i.x) - (2.f * i.y * i.y) );
	
	return Matrix44( iBasis, jBasis, kBasis, Vector3::ZERO );
}

float Quaternions::DotProduct( Quaternions const &a, Quaternions const &b )
{
	return Vector4::DotProduct( a.GetAsVector4(), b.GetAsVector4() );
}

