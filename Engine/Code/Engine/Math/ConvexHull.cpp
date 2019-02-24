#pragma once
#include "ConvexHull.hpp"
#include "Engine/Math/MathUtil.hpp"

ConvexHull::ConvexHull()
{

}

ConvexHull::~ConvexHull()
{

}

void ConvexHull::AddPlane( Plane3 const &newPlane )
{
	m_planes.push_back( newPlane );
}

bool ConvexHull::IsPointOutside( Vector3 const &testP, float floatDistanceErrorTolerance ) const
{
	for ( uint i = 0; i < m_planes.size(); i++ )
	{
		Plane3 const &thisPlane = m_planes[i];

		// If the point is in front side of this plane's normal
		float distFromPlane = thisPlane.GetDistanceFromPoint( testP );
		if( distFromPlane > floatDistanceErrorTolerance )
			return true;
	}

	return false;
}

