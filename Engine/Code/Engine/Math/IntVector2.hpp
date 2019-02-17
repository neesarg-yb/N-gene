#pragma once
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"

#include <string>

class IntVector2
{
public:
	int x;
	int y;

public:
	static const IntVector2 ZERO;
	static const IntVector2 ONE_ONE;
	static const IntVector2 MINUS_ONE_XY;

public:
	 IntVector2();
	 IntVector2( int x, int y );
	 IntVector2( Vector2& inVec2 );
	~IntVector2();

public:
	// Operators
	const IntVector2 operator +  ( const IntVector2& vecToAdd ) const;		 // vec2 + vec2
	const IntVector2 operator -  ( const IntVector2& vecToSubtract ) const;  // vec2 - vec2
	bool			 operator == ( const IntVector2& vecToCompare ) const;	 // IntVec2 == IntVec2 
	bool			 operator != ( const IntVector2& vecToCompare ) const;	 // IntVec2 != IntVec2 

	// Just for std::map< IntVector2, TypeB >
	bool			 operator <  ( const IntVector2& vecToCompate ) const;

public:
	float	GetLength() const;
	float	GetLengthSquared() const;

public:
	void	SetFromText( const char* text );
};


const IntVector2 Interpolate( const IntVector2& start, const IntVector2& end, float fractionTowardEnd );