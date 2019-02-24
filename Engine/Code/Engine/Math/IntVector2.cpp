#include "IntVector2.hpp"

const IntVector2 IntVector2::ZERO			= IntVector2(  0,  0 );
const IntVector2 IntVector2::ONE_ONE		= IntVector2(  1,  1 );
const IntVector2 IntVector2::MINUS_ONE_XY	= IntVector2( -1, -1 );

IntVector2::IntVector2()
{
	x = 0;
	y = 0;
}

IntVector2::IntVector2( int x, int y ) {
	this->x = x;
	this->y = y;
}

IntVector2::IntVector2( Vector2& inVec2 )
{
	x = (int) inVec2.x;
	y = (int) inVec2.y;
}

IntVector2::~IntVector2()
{
}

//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator + ( const IntVector2& vecToAdd ) const
{
	return IntVector2( (x + vecToAdd.x), (y + vecToAdd.y) );
}


//-----------------------------------------------------------------------------------------------
const IntVector2 IntVector2::operator-( const IntVector2& vecToSubtract ) const
{
	return IntVector2( (x - vecToSubtract.x), (y - vecToSubtract.y) );
}

bool IntVector2::operator == ( const IntVector2& vecToCompare ) const
{
	if( x == vecToCompare.x && y == vecToCompare.y ) {
		return true;
	}

	return false;
}

bool IntVector2::operator != ( const IntVector2& vecToCompare ) const
{
	if( x == vecToCompare.x && y == vecToCompare.y ) {
		return false;
	}

	return true;
}

bool IntVector2::operator < ( const IntVector2& vecToCompate ) const
{
	// Note! This comparison is there just so that I can use std:map with IntVector2
	//		 I compare Y-Axis first, and then X-Axis to have a consistent < behavior..

	IntVector2 const &left	= *this;
	IntVector2 const &right	= vecToCompate;

	if( left.y < right.y )
		return true;
	else if( left.y > right.y )
		return false;
	else if( left.x < right.x )
		return true;
	else //( left.x >= right.y )
		return false;
}

float IntVector2::GetLength() const
{
	return sqrtf( GetLengthSquared() );
}

float IntVector2::GetLengthSquared() const
{
	return (float)((x*x) + (y*y));
}

void IntVector2::SetFromText( const char* text )
{
	std::string inputStr = text;
	int firstCommaAt = (int) inputStr.find(',');
	
	std::string first(inputStr , 0 , firstCommaAt);
	::SetFromText( this->x , first.c_str() );
	std::string second(inputStr , firstCommaAt+1);
	::SetFromText( this->y , second.c_str() );
}


const IntVector2 Interpolate( const IntVector2& start, const IntVector2& end, float fractionTowardEnd )
{
	IntVector2 resultVec = IntVector2();
	resultVec.x = Interpolate( start.x , end.x , fractionTowardEnd );
	resultVec.y = Interpolate( start.y , end.y , fractionTowardEnd );

	return resultVec;
}
