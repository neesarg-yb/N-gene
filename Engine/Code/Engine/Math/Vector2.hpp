#pragma once

#include <math.h>
#include <string>
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector3.hpp"

# define M_PI           3.1415926535f  /* pi */

//-----------------------------------------------------------------------------------------------
class Vector2
{
public:
	// Construction/Destruction
	~Vector2() {}											// destructor: do nothing (for speed)
	 Vector2() {}											// default constructor: do nothing (for speed)
	 Vector2( const Vector2& copyFrom );					// copy constructor (from another vec2)
	explicit Vector2( float initialX, float initialY );				// explicit constructor (from x, y)

public:
	static Vector2 ZERO;
	static Vector2 TOP_DIR;
	static Vector2 LEFT_DIR;
	static Vector2 RIGHT_DIR;
	static Vector2 BOTTOM_DIR;

	// Operators
	const Vector2 operator + ( const Vector2& vecToAdd ) const;		// vec2 + vec2
	const Vector2 operator - ( const Vector2& vecToSubtract ) const;// vec2 - vec2
	const Vector2 operator * ( float uniformScale ) const;			// vec2 * float
	const Vector2 operator / ( float inverseScale ) const;			// vec2 / float

	void operator += ( const Vector2& vecToAdd );					// vec2 += vec2
	void operator -= ( const Vector2& vecToSubtract );				// vec2 -= vec2
	void operator *= ( const float uniformScale );					// vec2 *= float
	void operator /= ( const float uniformDivisor );				// vec2 /= float
	void operator =  ( const Vector2& copyFrom );					// vec2 = vec2
	bool operator == ( const Vector2& compare ) const;				// vec2 == vec2
	bool operator != ( const Vector2& compare ) const;				// vec2 != vec2

	friend const Vector2 operator*( float uniformScale, const Vector2& vecToScale );	// float * vec2

	float	GetLengthSquared() const;
	float	GetLength() const;
	Vector2 GetNormalized() const;
	float	NormalizeAndGetLength();
	float	GetOrientationDegrees() const;
	Vector3	GetAsVector3() const;

	static Vector2	MakeDirectionAtDegrees(float degree);
	static float	GetDistanceSquared(const Vector2& pos1, const Vector2& pos2);
	static float	GetDistance(const Vector2& pos1, const Vector2& pos2);
	static float	DotProduct( const Vector2& a, const Vector2& b );
	static float    DotProductForAngleDifference( float angleInDegrees );

	void SetFromText( const char* text );

public: 
	// NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;
};


// Gets the projected vector in the “projectOnto” direction, whose magnitude is the projected length of “vectorToProject” in that direction.
const Vector2 GetProjectedVector( const Vector2& vectorToProject, const Vector2& projectOnto );

// Returns the vector’s representation/coordinates in (i,j) space (instead of its original x,y space)
const Vector2 GetTransformedIntoBasis( const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ );

// Takes “vectorInBasis” in (i,j) space and returns the equivalent vector in [axis-aligned] (x,y) Cartesian space
const Vector2 GetTransformedOutOfBasis( const Vector2& vectorInBasis, const Vector2& oldBasisI, const Vector2& oldBasisJ );

// Decomposes “originalVector” into two component vectors, which add up to the original:
//   “vectorAlongI” is the vector portion in the “newBasisI” direction, and
//   “vectorAlongJ” is the vector portion in the “newBasisJ” direction.
void DecomposeVectorIntoBasis( const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ, Vector2& out_vectorAlongI, Vector2& out_vectorAlongJ );

const Vector2 Interpolate( const Vector2& start, const Vector2& end, float fractionTowardEnd );

Vector2 Reflect( const Vector2& incomingVector , const Vector2& normalVector );