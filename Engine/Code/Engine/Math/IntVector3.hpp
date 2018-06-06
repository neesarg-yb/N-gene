#pragma once

class Vector3;

class IntVector3
{
public:
	static IntVector3 ZERO;
	static IntVector3 ONE_ALL;		// = ( 1, 1, 1 )
	static IntVector3 UP;			// = ( 0, 1, 0 )
	static IntVector3 FRONT;		// = ( 0, 0, 1 )
	static IntVector3 RIGHT;		// = ( 1, 0, 0 )

public:
			  IntVector3() {};
			  IntVector3( const IntVector3& copyFrom );			
	 explicit IntVector3( int initialX, int initialY, int initialZ );		
			 ~IntVector3() {};


	// Operators
	const IntVector3 operator + ( const IntVector3& vecToAdd ) const;			// vec2 + vec2
	const IntVector3 operator - ( const IntVector3& vecToSubtract ) const;		// vec2 - vec2
	const IntVector3 operator * ( int uniformScale ) const;						// vec2 * int
	const IntVector3 operator / ( int inverseScale ) const;						// vec2 / int

	float		GetLength() const;
	Vector3		GetNormalized() const;

public:
	int x = 0;
	int y = 0;
	int z = 0;
};