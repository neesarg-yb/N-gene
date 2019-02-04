#pragma once
#include "ConvexHull.hpp"

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

bool ConvexHull::IsPointOutside( Vector3 const &testP ) const
{
	for ( uint i = 0; i < m_planes.size(); i++ )
	{
		Plane3 const &thisPlane = m_planes[i];

		// If the point is in front side of this plane's normal
		float distFromPlane = thisPlane.GetDistanceFromPoint( testP );
		if( distFromPlane > 0.f )
			return true;
	}

	return false;
}

