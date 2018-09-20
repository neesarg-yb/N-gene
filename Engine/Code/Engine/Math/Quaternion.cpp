#pragma once
#include "Quaternion.hpp"
#include "Engine/Math/MathUtil.hpp"

Quaternion Quaternion::IDENTITY = Quaternion( 1.f, Vector3::ZERO );

Quaternion::Quaternion()
{
	// Identity quaternion, by default values
}

Quaternion::Quaternion( float r, Vector3 const &i )
{
	this->r = r;
	this->i = i;
}

Quaternion::Quaternion( Vector3 const &axis, float rotationDegrees )
{
	float halfRotationRadians = DegreeToRadian( rotationDegrees ) * 0.5f;

	r	= cosf( -halfRotationRadians );
	i.x	= axis.x * sinf( -halfRotationRadians );
	i.y	= axis.y * sinf( -halfRotationRadians );
	i.z	= axis.z * sinf( -halfRotationRadians );
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
		float rSquare = r * r;
		float iLength = sqrtf( 1.f - rSquare );
		
		i = i.GetNormalized();
		i = i * iLength;
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
	// From: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

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

	Quaternion q;
	
	if ( tr > 0 ) 
	{ 
		float S = sqrtf(tr + 1.f) * 2.f;			// S = 4*qw
		q.r		= 0.25f * S;
		q.i.x	= (m21 - m12) / S;
		q.i.y	= (m02 - m20) / S;
		q.i.z	= (m10 - m01) / S;
	}
	else if ( (m00 > m11)&(m00 > m22) ) 
	{ 
		float S	= sqrt(1.f + m00 - m11 - m22) * 2; // S = 4*qx 
		q.r		= (m21 - m12) / S;
		q.i.x	= 0.25f * S;
		q.i.y	= (m01 + m10) / S;
		q.i.z	= (m02 + m20) / S;
	} 
	else if (m11 > m22) 
	{ 
		float S	= sqrt(1.f + m11 - m00 - m22) * 2; // S = 4*qy
		q.r		= (m02 - m20) / S;
		q.i.x	= (m01 + m10) / S; 
		q.i.y	= 0.25f * S;
		q.i.z	= (m12 + m21) / S; 
	} 
	else 
	{ 
		float S	= sqrt(1.f + m22 - m00 - m11) * 2; // S = 4*qz
		q.r		= (m10 - m01) / S;
		q.i.x	= (m02 + m20) / S;
		q.i.y	= (m12 + m21) / S;
		q.i.z	= 0.25f * S;
	}

	q.Normalize();

	return q;
}

float Quaternion::DotProduct( Quaternion const &a, Quaternion const &b )
{
	return Vector4::DotProduct( a.GetAsVector4(), b.GetAsVector4() );
}

Quaternion Quaternion::Slerp( Quaternion a, Quaternion const &b, float t )
{
	// References:
	//			(1) Book "3D Math Primer for Graphics and Game Development" by Fletcher Dunn and Ian Parberry
	//				-> Topic: [10.4.13] Quaternion Interpolation
	//			(2) YouTube Video: https://www.youtube.com/watch?v=x1aCcyD0hqE


	// Interpolation using solution: Qt = k0(Qa) + k1(Qb)
	float k0;
	float k1;

	// q and -q represents the same angle,
	// but we need both quaternions having positive dot product => Not in opposite directions
	// Determine that with dot product and if necessary, flip the direction of one of the quaternions..

	// DotProduct( a, b ) = Cos( omega )
	//
	//         .             
	//        /|             
	//       / |             Projection of a on b =  Cos( omega )
	//    a /  |                                                 
	//     /   |                    ( b/c length of both is ONE )
	//    /    |                                                 
	//   /omega|             
	//  /)_____|__________   
	//         b             
	float cosOmega = Quaternion::DotProduct( a, b );
	if( cosOmega < 0.f )
	{
		a.r		= -1.f * a.r;
		a.i.x	= -1.f * a.i.x;
		a.i.y	= -1.f * a.i.y;
		a.i.z	= -1.f * a.i.z;
		
		cosOmega = -1.f * cosOmega;
	}

	// If both quaternions are very close together => Dot Product is near ONE
	// Do normal lerp to overcome divide by zero risk
	float const dotProduct = cosOmega;
	if( dotProduct > 0.9999f )
	{
		// Do linear interpolation
		k0 = 1.f - t;
		k1 = t;
	}
	else
	{
		// Do spherical interpolation
		//
		// Get sin(omega)
		// Using trig identity: sin^2(omega) + cos^2(omega) = 1
		float sinOmega = sqrtf( 1.f - (cosOmega * cosOmega) );

		// Get angle omega using inverse of tan(omega)
		float omega = atan2f( sinOmega, cosOmega );

		// Value of 1 / sin(omega); we're gonna use it twice
		float oneOverSinOmega = 1.f / sinOmega;

		k0 = sinf( (1.f - t) * omega ) * oneOverSinOmega;
		k1 = sinf( t * omega ) * oneOverSinOmega;
	}

	// Interpolation
	Quaternion c;
	c.r		= (k0 * a.r)   + (k1 * b.r);
	c.i.x	= (k0 * a.i.x) + (k1 * b.i.x);
	c.i.y	= (k0 * a.i.y) + (k1 * b.i.y);
	c.i.z	= (k0 * a.i.z) + (k1 * b.i.z);

	return c;
}
