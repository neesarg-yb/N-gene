#pragma once
#include <vector>
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/ConvexHull.hpp"
#include "Engine/Renderer/RenderTypes.hpp"

typedef std::vector< Vector3 >		 VertexPositions;
typedef std::vector< uint >			 VertexIndices;
typedef std::vector< VertexIndices > Faces;

class ConvexPolyhedron
{
public:
	 ConvexPolyhedron();
	 ConvexPolyhedron( ConvexHull const &hull );
	~ConvexPolyhedron();

private:
	// Raw data
	ConvexHull		 m_hull;

	// Baked data
	VertexPositions	 m_vertices;							// Actual positions
	Faces			 m_faces;								// Indices of vertices
	bool			 m_isDirty	= true;						// Need to rebuild?

public:
	void AddPlane( Plane3 const &plane );
	void Rebuild();											// Calculates the baked data, again

	void DebugRenderVertices( float lifetime, float pointSize, eDebugRenderMode renderMode ) const;
	void DebugRenderVertexIndicesTag( float lifetime, float height, Vector3 const &cameraUp, Vector3 const &cameraRight ) const;

private:
	void PerformPlaneIntersections();						// Uses three plane intersection approach
	void SortFaceVerticesWinding( eWindOrder windOrder );

	bool GetIntersection( Plane3 const &p1, Plane3 const &p2, Plane3 const &p3, Vector3 &intersectionPoint_out ) const;
	void AddVertexForFace( Vector3 const &vert, uint faceIndex );

	// Helper methods for vertex sorting
	Plane3 MakeComparisionPlaneForPolygonEdge( Vector3 const &ePoint1, Vector3 const &ePoint2, Plane3 const &facePlane ) const;		// Makes a plane perpendicular of the face plane
	float  GetClosestPlaneForVertex( Vector3 const &vert, std::vector< Plane3 > const &fromPlanes, uint &planeIndex_out ) const;	// returns distance from the plane. (+) in direction of normal, (-) in opp. direction of normal
};
