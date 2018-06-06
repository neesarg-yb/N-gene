#pragma once

#include "MathUtil.hpp"
#include <math.h>

# define M_PI           3.1415926535f  /* pi */

//-----------------------------------------------------------------------------------------------
class Vector2
{
public:
	// Construction/Destruction
	~Vector2() {}											// destructor: do nothing (for speed)
	Vector2() {}											// default constructor: do nothing (for speed)
	Vector2( const Vector2& copyFrom );						// copy constructor (from another vec2)
	explicit Vector2( float initialX, float initialY );		// explicit constructor (from x, y)

															// Operators
	const Vector2 operator+( const Vector2& vecToAdd ) const;		// vec2 + vec2
	const Vector2 operator-( const Vector2& vecToSubtract ) const;	// vec2 - vec2
	const Vector2 operator*( float uniformScale ) const;			// vec2 * float
	const Vector2 operator/( float inverseScale ) const;			// vec2 / float
	void operator+=( const Vector2& vecToAdd );						// vec2 += vec2
	void operator-=( const Vector2& vecToSubtract );				// vec2 -= vec2
	void operator*=( const float uniformScale );					// vec2 *= float
	void operator/=( const float uniformDivisor );					// vec2 /= float
	void operator=( const Vector2& copyFrom );						// vec2 = vec2
	bool operator==( const Vector2& compare ) const;				// vec2 == vec2
	bool operator!=( const Vector2& compare ) const;				// vec2 != vec2

	friend const Vector2 operator*( float uniformScale, const Vector2& vecToScale );	// float * vec2

	float GetLengthSquared() const;
	float GetLength() const;
	Vector2 GetNormalized() const;
	float NormalizeAndGetLength();
	float GetOrientationDegrees() const;

	static Vector2 MakeDirectionAtDegrees(float degree);
	static float GetDistanceSquared(const Vector2& pos1, const Vector2& pos2);
	static float GetDistance(const Vector2& pos1, const Vector2& pos2);


public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;
};


