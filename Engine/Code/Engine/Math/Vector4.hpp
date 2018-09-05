#pragma once
class Vector3;

class Vector4
{
public:
	static Vector4 const ZERO;

public:
	Vector4() {}
	Vector4( float x, float y, float z, float w );
	Vector4( Vector3 xyzVec, float w );
	
	// Modifiers
	void	Clamp01();
	void	SetFromText( const char* text );

	// Const Methods
	float	GetLength() const;
	float	GetLengthSquared() const;
	Vector3 IgnoreW() const;
	Vector4 GetNormalized() const;

public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	float w = 0.f;

public:
	static float DotProduct( Vector4 const &a, Vector4 const &b );
};