#pragma once
#include "Plane3.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

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

Vector3 Plane3::VectorProjection( Vector3 const &vector )
{
	return ProjectVectorOnPlane( vector, normal );
}

Vector3 ProjectVectorOnPlane( Vector3 const &vector, Vector3 const &normal )
{
	// Calculations are based on,
	//	this link: http://www.euclideanspace.com/maths/geometry/elements/plane/lineOnPlane/index.htm
	//
	float normalLength = normal.GetLength();
	GUARANTEE_RECOVERABLE( normalLength != 0.f, "Warning: Projection calculations will be wrong b/c normal's length is ZERO." );

	Vector3 vecCrossNormal	= Vector3::CrossProduct( vector, normal ) / normalLength;
	Vector3 projection		= Vector3::CrossProduct( normal, vecCrossNormal ) / normalLength;

	return projection;
}
