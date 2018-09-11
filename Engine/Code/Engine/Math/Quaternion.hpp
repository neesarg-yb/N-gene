#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix44.hpp"

class Quaternion
{
public:
	 Quaternion();
	 Quaternion( Vector3 const &axis, float rotationDegrees );
	~Quaternion();

public:
	float	r = 1.f;			// Scaler Part:		w
	Vector3	i = Vector3::ZERO;	// Imaginary Part:	x, y, z

public:
	void inline	Invert() { *this = GetInverse(); }
	void		Normalize();
	float		GetMagnitude() const;
	float		GetMagnitudeSquared() const;
	bool		IsUnitQuaternion() const;

public:
	Quaternion	GetInverse	() const;
	Quaternion	Multiply	( Quaternion const b ) const;								// Note: q3 = q2.Multiply(q1) => q3 = q2*q1 => rotation by q1 happens first, and then rotation by q2
	Vector3		RotatePoint	( Vector3 const point ) const;

	Vector3		GetAsEuler()	const;
	Matrix44	GetAsMatrix44()	const;

	Vector3 inline	GetUp()			const { return RotatePoint( Vector3::UP ); }
	Vector3 inline	GetRight()		const { return RotatePoint( Vector3::RIGHT ); }
	Vector3 inline	GetFront()		const { return RotatePoint( Vector3::FRONT ); }
	Vector4 inline	GetAsVector4()	const { return Vector4( r, i.x, i.y, i.z ); }		// returns Vec4( r, i.x, i.j, i.z )

public:
	static Quaternion	FromEuler ( float x, float y, float z );
	static Quaternion	FromEuler ( Vector3 const &eulerInDegrees );
	static Quaternion	FromMatrix( Matrix44 const &mat44 );
	
	static float		DotProduct( Quaternion const &a, Quaternion const &b );
};