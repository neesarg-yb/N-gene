#pragma once

class IntVector2
{
public:
	 IntVector2();
	 IntVector2( int x, int y );
	~IntVector2();

	// Operators
	const IntVector2 operator +  ( const IntVector2& vecToAdd ) const;		 // vec2 + vec2
	const IntVector2 operator -  ( const IntVector2& vecToSubtract ) const;  // vec2 - vec2
	bool			 operator == ( const IntVector2& vecToCompare ) const;	 // IntVec2 == IntVec2 
	bool			 operator != ( const IntVector2& vecToCompare ) const;	 // IntVec2 != IntVec2 

	int x;
	int y;

private:

};