#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"

class Quaternions
{
public:
	 Quaternions();
	 Quaternions( Vector3 const &axis, float rotationDegrees );
	~Quaternions();

public:
	float	r = 1.f;			// Scaler Part:		w
	Vector3	i = Vector3::ZERO;	// Imaginary Part:	x, y, z

public:
	Quaternions GetInvert() const;
	Quaternions Multiply	( Quaternions const b ) const;				// Note: q3 = q2.Multiply(q1) => q3 = q2*q1 => rotation by q1 happens first, and then rotation by q2
	Vector3		RotatePoint	( Vector3 const point ) const;
};