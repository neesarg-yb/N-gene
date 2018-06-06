#pragma once
#include <math.h>
#include "IntVector3.hpp"
#include "Engine/Math/Vector3.hpp"

IntVector3 IntVector3::ZERO			= IntVector3( 0, 0, 0 );
IntVector3 IntVector3::ONE_ALL		= IntVector3( 1, 1, 1 );
IntVector3 IntVector3::UP			= IntVector3( 0, 1, 0 );
IntVector3 IntVector3::FRONT		= IntVector3( 0, 0, 1 );
IntVector3 IntVector3::RIGHT		= IntVector3( 1, 0, 0 );

IntVector3::IntVector3( const IntVector3& copyFrom )
{
	this->x = copyFrom.x;
	this->y = copyFrom.y;
	this->z = copyFrom.z;
}

IntVector3::IntVector3( int initialX, int initialY, int initialZ )
{
	this->x = initialX;
	this->y = initialY;
	this->z = initialZ;
}

const IntVector3 IntVector3::operator + ( const IntVector3& vecToAdd ) const
{
	return IntVector3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}

const IntVector3 IntVector3::operator - ( const IntVector3& vecToSubtract ) const
{
	return IntVector3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}

const IntVector3 IntVector3::operator * ( int uniformScale ) const
{
	return IntVector3( x * uniformScale, y * uniformScale, z * uniformScale );
}

const IntVector3 IntVector3::operator / ( int inverseScale ) const
{
	return IntVector3( x / inverseScale, y / inverseScale, z / inverseScale );
}

float IntVector3::GetLength() const
{
	const float squaredLength	= (float)( (x*x) + (y*y) + (z*z) );
	const float length			= sqrtf( squaredLength );

	return length;
}

Vector3 IntVector3::GetNormalized() const 
{
	const float lengthOfVec	= GetLength();

	return Vector3( x/lengthOfVec, y/lengthOfVec, z/lengthOfVec );
}