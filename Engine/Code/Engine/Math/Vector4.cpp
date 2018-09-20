#pragma once
#include "Vector4.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtil.hpp"

Vector4 const Vector4::ZERO = Vector4( 0.f, 0.f, 0.f, 0.f );

Vector4::Vector4( float x, float y, float z, float w )
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4( Vector3 xyzVec, float w )
{
	this->x = xyzVec.x;
	this->y = xyzVec.y;
	this->z = xyzVec.z;
	this->w = w;
}

void Vector4::Clamp01()
{
	this->x = ClampFloat01( x );
	this->y = ClampFloat01( y );
	this->z = ClampFloat01( z );
	this->w = ClampFloat01( w );
}

void Vector4::SetFromText( const char* text )
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
	
	nextStartIndex = commaAt + 1;
	commaAt = (int) inputStr.find(',' , nextStartIndex);
	length = commaAt - nextStartIndex;
	std::string fourth(inputStr , nextStartIndex , length);
	::SetFromText( fInput , fourth.c_str() );
	this->w = fInput;
}

float Vector4::GetLength() const
{
	float const squaredLength	= (x*x) + (y*y) + (z*z) + (w*w);
	float const length			= sqrtf( squaredLength );

	return length;
}

float Vector4::GetLengthSquared() const
{
	float const squaredLength	= (x*x) + (y*y) + (z*z) + (w*w);
	return squaredLength;
}

Vector3 Vector4::IgnoreW() const
{
	Vector3 toReturn( this->x, this->y, this->z );
	return  toReturn;
}

Vector4 Vector4::GetNormalized() const
{
	const float lengthOfVec	= GetLength();

	return Vector4( x/lengthOfVec, y/lengthOfVec, z/lengthOfVec, w/lengthOfVec );
}

bool Vector4::operator == ( Vector4 const b ) const
{
	return (x == b.x) && (y == b.y) && (z == b.z) && (w == b.w);
}

float Vector4::DotProduct( Vector4 const &a, Vector4 const &b )
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}
