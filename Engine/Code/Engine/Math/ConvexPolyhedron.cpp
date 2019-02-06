#pragma once
#include "ConvexPolyhedron.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

ConvexPolyhedron::ConvexPolyhedron( eWindOrder faceWindOrder /* = WIND_COUNTER_CLOCKWISE */ )
	: m_winding( faceWindOrder )
{

}

ConvexPolyhedron::ConvexPolyhedron( ConvexHull const &hull, eWindOrder faceWindOrder /* = WIND_COUNTER_CLOCKWISE */ )
	: m_hull( hull )
	, m_winding( faceWindOrder )
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

void ConvexPolyhedron::SetFaceWindingOrder( eWindOrder winding )
{
	m_winding = winding;
	m_isDirty = true;
}

void ConvexPolyhedron::Rebuild()
{
	// Get vertices on each planes
	PerformPlaneIntersections();

	// Sort vertices according to culling order
	SortFaceVerticesWinding( m_winding );

	// Baked data represents the Convex Hull accurately, now..
	m_isDirty = false;
}

void ConvexPolyhedron::DebugRenderVertices( float lifetime, float pointSize, float fontSize, Vector3 const &camUpDir, Vector3 const &camRightDir, eDebugRenderMode renderMode ) const
{
	for( uint i = 0; i < m_vertices.size(); i++ )
	{
		Vector3 const &vert = m_vertices[i];

		DebugRenderPoint( lifetime, pointSize, vert, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, renderMode );
		DebugRenderTag( lifetime, fontSize, vert, camUpDir, camRightDir, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("(%u)", i) );
	}
}

void ConvexPolyhedron::DebugRenderVertexIndicesTag( float lifetime, float height, Vector3 const &cameraUp, Vector3 const &cameraRight ) const
{
	for( uint fIdx = 0; fIdx < m_faces.size(); fIdx++ )
	{
		VertexIndices const &faceVertsIndices = m_faces[ fIdx ];

		Vector3 avgFacePos = Vector3::ZERO;
		for( uint vIdx = 0; vIdx < faceVertsIndices.size(); vIdx++ )
		{
			Vector3 vertex = m_vertices[ faceVertsIndices[vIdx] ];
			avgFacePos += vertex;
			vertex.y -= (height * (fIdx+1));
			DebugRenderTag( lifetime, height, vertex, cameraUp, cameraRight, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("f:%u v:%u", fIdx, vIdx) );
		}

		avgFacePos = avgFacePos / (float)faceVertsIndices.size();
		DebugRenderTag( lifetime, height, avgFacePos, cameraUp, cameraRight, RGBA_BLACK_COLOR, RGBA_BLACK_COLOR, Stringf("face %u", fIdx) );
	}
}

Mesh* ConvexPolyhedron::ConstructMesh( Rgba const &color ) const
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, false );

	for( int fId = 0; fId < m_faces.size(); fId++ )
	{
		// This face
		VertexIndices const	&faceVertIndices = m_faces[ fId ];
		Plane3 const		&facePlane		 = m_hull.m_planes[ fId ];

		// From this vert, let's triangulate the polygon
		Vector3 vert0 = m_vertices[ faceVertIndices[0] ];

		for( int v = 1; v < (faceVertIndices.size() - 1); v++ )
		{
			IntVector2	edgeVertIndices = IntVector2( faceVertIndices[ v ], faceVertIndices[ v+1 ] );
			Vector3		vertA			= m_vertices[ edgeVertIndices.x ];
			Vector3		vertB			= m_vertices[ edgeVertIndices.y ];
			
			mb.SetColor( color );
			mb.SetUV( Vector2::ZERO );
			mb.SetNormal( facePlane.normal );
			mb.PushVertex( vert0 );

			mb.SetColor( color );
			mb.SetUV( Vector2::ZERO );
			mb.SetNormal( facePlane.normal );
			mb.PushVertex( vertA );

			mb.SetColor( color );
			mb.SetUV( Vector2::ZERO );
			mb.SetNormal( facePlane.normal );
			mb.PushVertex( vertB );
		}
	}

	mb.End();

	return mb.ConstructMesh<Vertex_Lit>();
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
	// For each face
	for ( uint fInd = 0; fInd < m_faces.size(); fInd++ )
	{
		Plane3 const		&facePlane		 = m_hull.m_planes[ fInd ];
		VertexIndices const &faceVertIndices = m_faces[ fInd ];
		
		// Add first two vertices in sorted list
		VertexIndices sortedFaceVertsInd;
		sortedFaceVertsInd.push_back( faceVertIndices[0] );
		sortedFaceVertsInd.push_back( faceVertIndices[1] );

		// Add first comparison plane to the list
		std::vector< Plane3 > comparisionPlanes;
		uint	firstVertInd		= sortedFaceVertsInd[0];
		uint	secondVertInd		= sortedFaceVertsInd[1];
		Plane3	firstEdgeCmpPlane	= MakeComparisionPlaneForPolygonEdge( m_vertices[ firstVertInd ], m_vertices[ secondVertInd ], facePlane );
		comparisionPlanes.push_back( firstEdgeCmpPlane );

		// Let's insert vertices one by one, to form a sorted winding order
		for( uint vInd = 2; vInd < faceVertIndices.size(); vInd++ )
		{
			uint	vertexIndex	= faceVertIndices[ vInd ];
			Vector3 thisVert	= m_vertices[ vertexIndex ];

			uint closestPlaneIdx;
			float multiplier	= (windOrder != WIND_COUNTER_CLOCKWISE) ? -1.f : +1.f;
			float distance		= multiplier * GetClosestPlaneForVertex( thisVert, comparisionPlanes, closestPlaneIdx );

			if( distance >= 0.f )
			{
				// Point is on the front side of the normal of cmpPlane
				// i.e. add the point at the end of second vertex of this edge
				uint newVertAtIndex = closestPlaneIdx + 2U;
				sortedFaceVertsInd.insert( sortedFaceVertsInd.begin() + newVertAtIndex, vertexIndex );
			}
			else
			{
				// Point is on the back side of the normal of cmpPlane
				// i.e. add the point in between the two vertices forming this edge
				uint newVertAtIndex = closestPlaneIdx + 1U;
				sortedFaceVertsInd.insert( sortedFaceVertsInd.begin() + newVertAtIndex, vertexIndex );
			}
			
			// Let's add a comparison plane for the second and third vertices
			Vector3 vert2 = m_vertices[ sortedFaceVertsInd[closestPlaneIdx + 1U] ];
			Vector3 vert3 = m_vertices[ sortedFaceVertsInd[closestPlaneIdx + 2U] ];
			Plane3 comparisionPlane2 = MakeComparisionPlaneForPolygonEdge( vert2, vert3, facePlane );
			comparisionPlanes.insert( comparisionPlanes.begin() + closestPlaneIdx + 1, comparisionPlane2 );
			
			if( distance < 0.f )
			{
				// It means that we added a new vertex in between old two (which formed the edge)
				// So, we also need to update comparison plane for the first and newly added second vertices
				// Plane for 2nd and 3rd vertices is already updated
				Vector3 vert1 = m_vertices[ sortedFaceVertsInd[closestPlaneIdx + 0U] ];
				Plane3 comparisionPlane1 = MakeComparisionPlaneForPolygonEdge( vert1, vert2, facePlane );
				comparisionPlanes[ closestPlaneIdx ] = comparisionPlane1;
			}
		}

		// Set the sorted vert indices for this face
		m_faces[ fInd ] = sortedFaceVertsInd;
	}
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

Plane3 ConvexPolyhedron::MakeComparisionPlaneForPolygonEdge( Vector3 const &ePoint1, Vector3 const &ePoint2, Plane3 const &facePlane ) const
{
	Vector3 p1to2Dir = (ePoint2 - ePoint1).GetNormalized();
	Vector3 normal	 = Vector3::CrossProduct( p1to2Dir, facePlane.normal );

	return Plane3( normal, ePoint1 );
}

float ConvexPolyhedron::GetClosestPlaneForVertex( Vector3 const &vert, std::vector< Plane3 > const &fromPlanes, uint &planeIndex_out ) const
{
	GUARANTEE_RECOVERABLE( fromPlanes.size() != 0U, "Error: Can't operate without any provided planes!" );

	uint idxWithLeastDist = 0U;
	float leastDistSigned = FLT_MAX;

	for (uint pIdx = 0; pIdx < fromPlanes.size(); pIdx++)
	{
		Plane3 const &thisPlane = fromPlanes[ pIdx ];

		float distance = thisPlane.GetDistanceFromPoint( vert );
		if( fabsf(distance) < fabsf(leastDistSigned) )
		{
			leastDistSigned  = distance;
			idxWithLeastDist = pIdx;
		}
	}

	planeIndex_out = idxWithLeastDist;
	return leastDistSigned;
}
