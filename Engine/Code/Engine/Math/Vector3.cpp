#include "Vector3.hpp"

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

Vector3 Vector3::ZERO = Vector3( 0.f, 0.f, 0.f );