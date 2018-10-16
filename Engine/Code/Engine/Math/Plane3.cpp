#pragma once
#include "Plane3.hpp"

Plane3::Plane3()
{

}

Plane3::Plane3( Vector3 normal, float d )
{
	this->normal = normal;
	this->d		 = d;
}

Plane3::Plane3( Vector3 normal, Vector3 pointOnPlane )
{
	this->normal =  normal;
	this->d		 = (normal.x * pointOnPlane.x) + (normal.y * pointOnPlane.y) + (normal.z * pointOnPlane.z);
}

Plane3::~Plane3()
{

}

float Plane3::GetDistanceFromPoint( Vector3 const &point )
{
	float distance = (normal.x * point.x) + (normal.y * point.y) + (normal.z * point.z) - d;

	return distance;
}

bool Plane3::IsValid() const
{
	if( normal == Vector3::ZERO )
		return false;
	else
		return true;
}
