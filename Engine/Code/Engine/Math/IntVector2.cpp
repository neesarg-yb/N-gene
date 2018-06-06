#include "IntVector2.hpp"


IntVector2::IntVector2()
{
	x = 0;
	y = 0;
}

IntVector2::IntVector2( int x, int y ) {
	this->x = x;
	this->y = y;
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