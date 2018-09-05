#pragma once
#include "Quaternion.hpp"
#include "Engine/Math/MathUtil.hpp"

Quaternion::Quaternion()
{
	// Identity quaternion, by default values
}

Quaternion::Quaternion( Vector3 const &axis, float rotationDegrees )
{
	float halfRotationRadians = DegreeToRadian( rotationDegrees ) * 0.5f;

	r	= cosf( halfRotationRadians );
	i.x	= axis.x * sinf( halfRotationRadians );
	i.y	= axis.y * sinf( halfRotationRadians );
	i.z	= axis.z * sinf( halfRotationRadians );
}

Quaternion::~Quaternion()
{

}

void Quaternion::Normalize()
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

float Quaternion::GetMagnitude() const
{
	return sqrtf( GetMagnitudeSquared() );
}

float Quaternion::GetMagnitudeSquared() const
{
	return DotProduct( *this, *this );
}

bool Quaternion::IsUnitQuaternion() const
{
	return ( 1.f == GetMagnitudeSquared() );
}

Quaternion Quaternion::GetInverse() const
{
	Quaternion qInvert;

	qInvert.r = r;
	qInvert.i = i * -1.f;

	return qInvert;
}

Quaternion Quaternion::Multiply( Quaternion const b ) const
{
	Quaternion ab; // = a.Multiply( b )

	ab.r = ( b.r * r ) - Vector3::DotProduct( i, b.i );
	ab.i = ( i * b.r ) + ( b.i * r ) + Vector3::CrossProduct( i, b.i );

	return ab;
}

Vector3 Quaternion::RotatePoint( Vector3 const point ) const
{
	// Derived equation comes from: Rotation through multiplication of two quaternions
	//
	// Quaternion p;
	// p.r = 0.f;
	// p.i = point;
	//
	// return ( q * p * q.GetInvert() ).i;				// Order of multiplication goes left to right -->

	// This is for q * p * qInverse
	// 
	// Vector3 iPointCross = Vector3::CrossProduct(i, point);
	// Vector3 pRotated	= point 
	// 					+ ( iPointCross * 2.f * r ) 
	// 					+ ( Vector3::CrossProduct(i, iPointCross) * 2.f );
	// 
	// return pRotated;

	// But just like how our Rotation Matrix Multiplication gets applied in reverse order (Right to Left)
	// Quaternion' sandwich multiplication also needs to be flipped..
	//
	// Thus: return ( q.GetInverse() * p * q ).i
	Quaternion p;
	p.r = 0.f;
	p.i = point;

	Quaternion const qInv	=  this->GetInverse();
	Quaternion const &q	= *this;

	return qInv.Multiply( p ).Multiply( q ).i;
}

Vector3 Quaternion::GetAsEuler() const
{
	Matrix44 mat44 = GetAsMatrix44();

	return mat44.GetEulerRotation();
}

Matrix44 Quaternion::GetAsMatrix44() const
{
	float const ix2 = i.x * i.x;
	float const iy2 = i.y * i.y;
	float const iz2 = i.z * i.z;
	
	Vector3 iBasis = Vector3( 1.f -	(2.f * iy2)		  - (2.f * iz2),
									(2.f * i.x * i.y) - (2.f * i.z * r),
									(2.f * i.x * i.z) + (2.f * i.y * r) );
	Vector3 jBasis = Vector3(		(2.f * i.x * i.y) + (2.f * i.z * r),
							 1.f -	(2.f * ix2)		  - (2.f * iz2),
									(2.f * i.y * i.z) - (2.f * i.x * r) );
	Vector3 kBasis = Vector3(		(2.f * i.x * i.z) - (2.f * i.y * r),
									(2.f * i.y * i.z) + (2.f * i.x * r),
							 1.f -	(2.f * ix2)		  - (2.f * iy2) );
	
	return Matrix44( iBasis, jBasis, kBasis, Vector3::ZERO );
}

Quaternion Quaternion::FromEuler( float x, float y, float z )
{
	Quaternion rotX = Quaternion( Vector3::RIGHT, x );
	Quaternion rotY = Quaternion( Vector3::UP, y );
	Quaternion rotZ = Quaternion( Vector3::FRONT, z );

	Quaternion rotZXY = rotZ.Multiply( rotX ).Multiply( rotY );
	return rotZXY;
}

Quaternion Quaternion::FromEuler( Vector3 const &eulerInDegrees )
{
	return FromEuler( eulerInDegrees.x, eulerInDegrees.y, eulerInDegrees.z );
}

Quaternion Quaternion::FromMatrix( Matrix44 const &mat44 )
{
	/*
	Formula expects matrix,								My matrix is,
		| m00  m01  m02 |   | Ix  Iy  Iz |						| Ix  Jx  Kx |
		| m10  m11  m12 | = | Jx  Jy  Jz |			BUT			| Iy  Jy  Ky |
		| m20  m21  m22 |   | Kx  Ky  Kz |						| Iz  Jz  Kz |
	*/

	float const m00	= mat44.Ix;
	float const m11	= mat44.Jy;
	float const m22	= mat44.Kz;
	float const tr	= m00 + m11 + m22;

	float const m21	= mat44.Ky;
	float const m12	= mat44.Jz;
	float const m02	= mat44.Iz;
	float const m20	= mat44.Kx;
	float const m10	= mat44.Jx;
	float const m01	= mat44.Iy;

//	Forseth's way
//
//	Quaternion q;
// 	if (tr >= 0.0f) 
// 	{
// 		float s		= sqrtf(tr + 1.0f) * 2.0f;
// 		float is	= 1.0f / s;
// 
// 		q.r		= 0.25f * s;
// 		q.i.x	= (m21 - m12) * is;
// 		q.i.y	= (m02 - m20) * is;
// 		q.i.z	= (m10 - m01) * is;
// 	} 
// 	else if ((m00 > m11) & (m00 > m22)) 
// 	{
// 		float s		= sqrtf( 1.0f + m00 - m11 - m22 ) * 2.0f;
// 		float is	= 1.0f / s;
// 
// 		q.r		= (m21 - m12) * is;
// 		q.i.x	= 0.25f * s;
// 		q.i.y	= (m01 + m10) * is;
// 		q.i.z	= (m02 + m20) * is;
// 	} 
// 	else if (m11 > m22) 
// 	{
// 		float s		= sqrtf( 1.0f + m11 - m00 - m22 ) * 2.0f;
// 		float is	= 1.0f / s;
// 
// 		q.r		= (m02 - m20) * is;
// 		q.i.x	= (m01 + m10) * is;
// 		q.i.y	= 0.25f * s;
// 		q.i.z	= (m12 + m21) * is;
// 	} 
// 	else 
// 	{
// 		float s		= sqrtf( 1.0f + m22 - m00 - m11 ) * 2.0f;
// 		float is	= 1.0f / s;
// 		q.r		= (m10 - m01) * is;
// 		q.i.x	= (m02 + m20) * is;
// 		q.i.y	= (m12 + m21) * is;
// 		q.i.z	= 0.25f * s;
// 	}


	// From: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	Quaternion q;

	q.r		= sqrtf(1 + m00 + m11 + m22) * 0.5f ;
	q.i.x	= (m21 - m12) / (4.f * q.r);
	q.i.y	= (m02 - m20) / (4.f * q.r);
	q.i.z	= (m10 - m01) / (4.f * q.r);

	q.Normalize();

	return q;
}

float Quaternion::DotProduct( Quaternion const &a, Quaternion const &b )
{
	return Vector4::DotProduct( a.GetAsVector4(), b.GetAsVector4() );
}

