#pragma once
#include "ConvexPolyhedron.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

ConvexPolyhedron::ConvexPolyhedron()
{

}

ConvexPolyhedron::ConvexPolyhedron( ConvexHull const &hull )
	: m_hull( hull )
{

}

ConvexPolyhedron::~ConvexPolyhedron()
{

}

void ConvexPolyhedron::AddPlane( Plane3 const &plane )
{
	m_hull.AddPlane( plane );

	// All the baked data might be inaccurate, now..
	m_isDirty = true;
}

void ConvexPolyhedron::Rebuild()
{
	// Get vertices on each planes
	PerformPlaneIntersections();

	// Sort vertices according to culling order
	SortFaceVerticesWinding( WIND_COUNTER_CLOCKWISE );

	// Baked data represents the Convex Hull accurately, now..
	m_isDirty = false;
}

void ConvexPolyhedron::DebugRenderVertices( float lifetime, float pointSize, eDebugRenderMode renderMode ) const
{
	for each (Vector3 vert in m_vertices)
		DebugRenderPoint( lifetime, pointSize, vert, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, renderMode );
}

void ConvexPolyhedron::PerformPlaneIntersections()
{
	uint const numPlanes = m_hull.GetPlaneCount();
	GUARANTEE_OR_DIE( numPlanes >= 4, "Error: Convex Polyhedron needs at least four planes to operate.." );

	// Clear previously baked data
	m_faces.clear();
	m_vertices.clear();

	// One (Convex Polyhedron) face per a (Convex Hull) plane
	for( uint i = 0; i < numPlanes; i++ )
		m_faces.push_back( VertexIndices() );

	// Start the intersection procedure
	for( uint i = 0; i < numPlanes-2; i++ )
	{
		for( uint j = 0; j < numPlanes-1; j++ )
		{
			for( uint k = 0; k < numPlanes; k++ )
			{
				// Skip if these are not distinct planes 
				bool distinctPlanes = (i != j) && (i != k) && (j != k);
				if( !distinctPlanes )
					continue;

				// Get intersection point
				Plane3 const &iPlane = m_hull.m_planes[i];
				Plane3 const &jPlane = m_hull.m_planes[j];
				Plane3 const &kPlane = m_hull.m_planes[k];

				Vector3 intersection;
				bool intersectsAtOnePoint = GetIntersection( iPlane, jPlane, kPlane, intersection );

				// Skip if,
				//  1. Plane does not intersect [OR]
				//  2. The intersection point is outside the convex hull
				if( !intersectsAtOnePoint || m_hull.IsPointOutside( intersection ) )
					continue;

				// Add the vertex index to face(s)
				AddVertexForFace( intersection, i );
				AddVertexForFace( intersection, j );
				AddVertexForFace( intersection, k );
			}
		}
	}
}

void ConvexPolyhedron::SortFaceVerticesWinding( eWindOrder windOrder )
{
	UNUSED( windOrder );
}

bool ConvexPolyhedron::GetIntersection( Plane3 const &p1, Plane3 const &p2, Plane3 const &p3, Vector3 &intersectionPoint_out ) const
{
	Vector3 const &n1 = p1.normal;
	Vector3 const &n2 = p2.normal;
	Vector3 const &n3 = p3.normal;

	// Triple scaler product
	Vector3 n2n3Cross	= Vector3::CrossProduct( n2, n3 );
	float	dotProduct	= Vector3::DotProduct( n1, n2n3Cross );

	if( AreEqualFloats( dotProduct, 0.f, 4 ) )
		return false;

	Vector3 n3n1Cross = Vector3::CrossProduct( n3, n1 );
	Vector3 n1n2Cross = Vector3::CrossProduct( n1, n2 );
	Vector3 numerator = (n2n3Cross * p1.d) + (n3n1Cross * p2.d) + (n1n2Cross * p3.d);

	intersectionPoint_out = (numerator / dotProduct);
	return true;
}

void ConvexPolyhedron::AddVertexForFace( Vector3 const &vert, uint faceIndex )
{
	// Get index for the vertex
	uint vIndex		= 0U;
	bool vertFound	= false;
	for( uint i = 0; i < m_vertices.size(); i++ )
	{
		if( m_vertices[i] != vert )
			continue;

		// Found the vertex
		vertFound = true;
		vIndex = i;
		break;
	}

	if( vertFound == false )
	{
		// Add new vertex in the list
		m_vertices.push_back( vert );

		// Set the index
		vIndex = (uint)m_vertices.size() - 1U;
	}

	// Add the vIndex to face
	bool faceContainsVert	 = false;
	VertexIndices &faceVerts = m_faces[ faceIndex ];
	for( uint i = 0; i < faceVerts.size(); i++ )
	{
		if( faceVerts[i] != vIndex )
			continue;

		// Found the vertex index
		faceContainsVert = true;
		break;
	}

	if( faceContainsVert == false )
		faceVerts.push_back( vIndex );
}
