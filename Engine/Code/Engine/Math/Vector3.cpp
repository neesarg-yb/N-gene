#pragma once
#include <math.h>
#include "Vector3.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Matrix44.hpp"

Vector3 Vector3::ZERO		= Vector3( 0.f, 0.f, 0.f );
Vector3 Vector3::ONE_ALL	= Vector3( 1.f, 1.f, 1.f );
Vector3 Vector3::UP			= Vector3( 0.f, 1.f, 0.f );
Vector3 Vector3::FRONT		= Vector3( 0.f, 0.f, 1.f );
Vector3 Vector3::RIGHT		= Vector3( 1.f, 0.f, 0.f );

Vector3::Vector3( const Vector3& copyFrom )
{
	this->x = copyFrom.x;
	this->y = copyFrom.y;
	this->z = copyFrom.z;
}

Vector3::Vector3( float initialX, float initialY, float initialZ )
{
	this->x = initialX;
	this->y = initialY;
	this->z = initialZ;
}

const Vector3 Vector3::operator + ( const Vector3& vecToAdd ) const
{
	return Vector3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}

const Vector3 Vector3::operator - ( const Vector3& vecToSubtract ) const
{
	return Vector3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}

const Vector3 Vector3::operator * ( float uniformScale ) const
{
	return Vector3( x * uniformScale, y * uniformScale, z * uniformScale );
}

const Vector3 Vector3::operator / ( float inverseScale ) const
{
	return Vector3( x / inverseScale, y / inverseScale, z / inverseScale );
}

bool Vector3::operator == ( const Vector3& vecToCompare ) const
{
	return (x == vecToCompare.x) && (y == vecToCompare.y) && (z == vecToCompare.z);
}

bool Vector3::operator != ( const Vector3& vecToCompare ) const
{
	return (x != vecToCompare.x) || (y != vecToCompare.y) || (z != vecToCompare.z);
}

float Vector3::GetLength() const
{
	const float squaredLength	= (x*x) + (y*y) + (z*z);
	const float length			= sqrtf( squaredLength );

	return length;
}

Vector3 Vector3::GetNormalized() const 
{
	const float lengthOfVec	= GetLength();

	return Vector3( x/lengthOfVec, y/lengthOfVec, z/lengthOfVec );
}

Vector3 Vector3::CrossProduct( const Vector3& first_vec, const Vector3& second_vec )
{
	Vector3 toReturn;

	toReturn.x = (first_vec.y * second_vec.z) - (first_vec.z * second_vec.y);
	toReturn.y = (first_vec.z * second_vec.x) - (first_vec.x * second_vec.z);
	toReturn.z = (first_vec.x * second_vec.y) - (first_vec.y * second_vec.x);

	return toReturn;
}

float Vector3::DotProduct( const Vector3& first_vec, const Vector3& second_vec )
{
	return (first_vec.x * second_vec.x) + (first_vec.y * second_vec.y) + (first_vec.z * second_vec.z);
}


void Vector3::SetFromText( const char* text )
{
	std::string inputStr = text;
	float fInput;

	int nextStartIndex = 0;
	int commaAt = (int) inputStr.find(',');
	int length = commaAt - nextStartIndex;
	std::string first(inputStr , nextStartIndex , length);
	::SetFromText( fInput , first.c_str() );
	this->x = fInput;

	nextStartIndex = commaAt + 1;
	commaAt = (int) inputStr.find(',' , nextStartIndex);
	length = commaAt - nextStartIndex;
	std::string second(inputStr , nextStartIndex , length);
	::SetFromText( fInput , second.c_str() );
	this->y = fInput;

	nextStartIndex = commaAt + 1;
	commaAt = (int) inputStr.find(',' , nextStartIndex);
	length = commaAt - nextStartIndex;
	std::string third(inputStr , nextStartIndex , length);
	::SetFromText( fInput , third.c_str() );
	this->z = fInput;
}

Vector3 PolarToCartesian( float radius, float rotation, float altitude )
{
	float x = radius * CosDegree( rotation ) * SinDegree( altitude );
	float z = radius * SinDegree( rotation ) * SinDegree( altitude );
	float y = radius * CosDegree( altitude );

	return Vector3( x, y, z );
}

const Vector3 Interpolate( const Vector3& start, const Vector3& end, float fractionTowardEnd )
{
	Vector3 resultVec;

	resultVec.x = Interpolate( start.x , end.x , fractionTowardEnd );
	resultVec.y = Interpolate( start.y , end.y , fractionTowardEnd );
	resultVec.z = Interpolate( start.z , end.z , fractionTowardEnd );

	return resultVec;
}