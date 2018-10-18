#pragma once
#include "AABB3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"

AABB3::AABB3()
{
	mins = Vector3::ZERO;
	maxs = Vector3::ZERO;
}

AABB3::AABB3( Vector3 const &min, Vector3 const &max )
{
	this->mins = min;
	this->maxs = max;
}

AABB3::AABB3( Vector3 const &center, float xDim, float yDim, float zDim )
{
	Vector3 halfDim = Vector3( xDim, yDim, zDim ) * 0.5f;
	
	this->mins		= center - halfDim;
	this->maxs		= center + halfDim;
}

AABB3::AABB3( float minX, float minY, float minZ, float maxX, float maxY, float maxZ )
{
	this->mins = Vector3( minX, minY, minZ );
	this->maxs = Vector3( maxX, maxY, maxZ );
}

AABB3::~AABB3()
{

}

float AABB3::GetDistanceFromPoint( Vector3 const &point ) const
{
	Vector3 closestPoint = GetClosestPointInsideBounds( point );

	return (point - closestPoint).GetLength();
}

bool AABB3::IsPointInsideMe( Vector3 const &point ) const
{
	bool biggerThanMins		= (point.x >= mins.x) && (point.y >= mins.y) && (point.z >= mins.z);
	bool smallerThanMaxs	= (point.x <= maxs.x) && (point.y <= maxs.y) && (point.z <= maxs.z);

	if( biggerThanMins && smallerThanMaxs )
		return true;
	else
		return false;
}

Vector3 AABB3::GetClosestPointInsideBounds( Vector3 const &from ) const
{
	Vector3 closestPoint;
	closestPoint.x = ClampFloat( from.x, mins.x, maxs.x );
	closestPoint.y = ClampFloat( from.y, mins.y, maxs.y );
	closestPoint.z = ClampFloat( from.z, mins.z, maxs.z );

	return closestPoint;
}

void AABB3::GetSixPlanes( Plane3 *planeArray ) const
{
	float z = maxs.z - mins.z;
	float y = maxs.y - mins.y;
	float x = maxs.x - mins.x;

	// Planes passing through mins
	Vector3 yxPlaneNormal = Vector3( 0.f, 0.f, -z );
	Vector3 zyPlaneNormal = Vector3( -x, 0.f, 0.f );
	Vector3 xzPlaneNormal = Vector3( 0.f, -y, 0.f );
	planeArray[0] = Plane3( yxPlaneNormal, mins );
	planeArray[1] = Plane3( zyPlaneNormal, mins );
	planeArray[2] = Plane3( xzPlaneNormal, mins );

	// Planes passing through maxs
	yxPlaneNormal = Vector3( 0.f, 0.f, z );
	zyPlaneNormal = Vector3( x, 0.f, 0.f );
	xzPlaneNormal = Vector3( 0.f, y, 0.f );
	planeArray[3] = Plane3( yxPlaneNormal, maxs );
	planeArray[4] = Plane3( zyPlaneNormal, maxs );
	planeArray[5] = Plane3( xzPlaneNormal, maxs );
}
