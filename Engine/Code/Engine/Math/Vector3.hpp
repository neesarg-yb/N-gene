#pragma once

class Vector2;

class Vector3
{
public:
	static Vector3 ZERO;
	static Vector3 ONE_ALL;
	static Vector3 UP;			// = ( 0, 1, 0 )
	static Vector3 FRONT;		// = ( 0, 0, 1 )
	static Vector3 RIGHT;		// = ( 1, 0, 0 )

public:
			  Vector3() {};
			  Vector3( const Vector3& copyFrom );			
	 explicit Vector3( float initialX, float initialY, float initialZ );		
			 ~Vector3() {};


	// Operators
	const Vector3 operator +  ( const Vector3& vecToAdd ) const;			// vec2 + vec2
	const Vector3 operator -  ( const Vector3& vecToSubtract ) const;		// vec2 - vec2
	const Vector3 operator *  ( float uniformScale ) const;					// vec2 * float
	const Vector3 operator /  ( float inverseScale ) const;					// vec2 / float
		  void	  operator += ( const Vector3& vetToAdd );					// vec3_a = vec3_a + vec3_b
		  void	  operator -= ( const Vector3& vetToSubtract );				// vec3_a = vec3_a - vec3_b
		  bool	  operator == ( const Vector3& vecToCompare ) const;		// vec2 == vec2
		  bool	  operator != ( const Vector3& vecToCompare ) const;		// vec2 != vec2

	float			GetLength() const;
	Vector3			GetNormalized() const;
	Vector3			GetAsDirection() const;		// From Euler Rotation to Direction Vector
	void			GetTangentAndBitangent( Vector3 *tangent, Vector3 *bitangent = nullptr ) const;	// If passed nullptr, it ignores that argument
	Vector2			IgnoreZ() const;

	// Producers
	static Vector3	CrossProduct( const Vector3& first_vec, const Vector3& second_vec );
	static float	DotProduct	( const Vector3& first_vec, const Vector3& second_vec );

	void			SetFromText( const char* text );

public:
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};

Vector3 PolarToCartesian( float radius, float rotation, float altitude );	// rotation = 0 starts at x-axis arrow; altitude = 0 starts at y-axis arrow


const Vector3 Interpolate( const Vector3& start, const Vector3& end, float fractionTowardEnd );