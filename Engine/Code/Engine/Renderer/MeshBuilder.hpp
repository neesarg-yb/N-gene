#pragma once
#include <functional>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Math/IntVector2.hpp"


class MeshBuilder
{
public:
	Vertex_Master					m_stamp;
	std::vector< Vertex_Master >	m_vertices;
	std::vector< unsigned int >		m_indices;
	DrawInstruction					m_drawInstruction;

public:
	 MeshBuilder() {};
	~MeshBuilder() {};

public:
	void	Begin( ePrimitiveType primType, bool useIndices );
	void	End();

	// Set the Vertices
	void			SetColor	( Rgba	  const &color );
	void			SetUV		( Vector2 const &uv	);
	void			SetUV		( float x, float y );
	void			SetNormal	( Vector3 const &normal );
	void			SetNormal	( float x, float y, float z );
	void			SetTangent4	( Vector4 const &tangent4 );
	void			SetTangent4	( float x, float y, float z, float w );
	unsigned int	PushVertex	( Vector3 const &position );	// Pushes the stampVertex_3DPCU, returns the index of Vertex

	// Set the Indices
	void	AddFace( unsigned int idx1, unsigned int idx2, unsigned int idx3 );

	// Create Mesh with data stored..
	template <typename VERTTYPE>
	Mesh* ConstructMesh() const									// Call End() before CreateMesh
	{
		Mesh			*mesh				= new Mesh();
		unsigned int	 vCount				= (unsigned int) m_vertices.size();
		VERTTYPE		*tempVertexBuffer	= (VERTTYPE *) malloc( sizeof(VERTTYPE) * vCount );

		// Fill the new temp vertexBuffer of passed VERTTYPE	
		for( unsigned int i = 0; i < vCount; i++ )
			tempVertexBuffer[i] = VERTTYPE( m_vertices[i] );

		// Use the new vertexBuffer, now
		mesh->SetVertices	  <VERTTYPE>( (unsigned int)		  vCount, tempVertexBuffer );
		mesh->SetIndices				( (unsigned int)m_indices.size(), m_indices.data() );
		mesh->SetDrawInstruction( m_drawInstruction );

		// temp VertexBuffer's work is done, free it
		free( tempVertexBuffer );

		return mesh;
	}
	
	
public:
	void			AddPlane	( Vector2 const &xySize, Vector3 const &centerPos, Rgba const &color = RGBA_WHITE_COLOR, const AABB2 &uvBounds = AABB2::ONE_BY_ONE );	// Adds a plane on existing MeshBuilder
	void			AddPlane	( AABB2 const &drawBounds, float const &zPosition, AABB2 const &uvBounds, Rgba const &color = RGBA_WHITE_COLOR );
	void			AddCube		( Vector3 const &size, Vector3 const &centerPos = Vector3::ZERO, Rgba const &color = RGBA_WHITE_COLOR, const AABB2& uvTop = AABB2::ONE_BY_ONE, const AABB2& uvSide = AABB2::ONE_BY_ONE, const AABB2& uvBottom = AABB2::ONE_BY_ONE );	// Adds a cube on existing MeshBuilder
	void			AddSphere	( float radius, unsigned int wedges, unsigned int slices, Vector3 centerPos = Vector3::ZERO, Rgba const &color = RGBA_WHITE_COLOR );	// Adds a sphere on existing MeshBuilder
	void			AddCylinder	( float radius, uint cuts, float length, Vector3 const &centerPos, Rgba const &color = RGBA_WHITE_COLOR );
	void			AddMeshFromSurfacePatch ( std::function< Vector3( float , float ) > SurfacePatch, Vector2 uvRangeMin, Vector2 uvRangeMax, IntVector2 sampleFrequency, Rgba const &color = RGBA_WHITE_COLOR );

	void			SetVertexPositionsRelativeTo( Vector3 pivotPosition );		// Subtracts the pivotPosition from every vertex position

public:
	static Mesh*	CreatePlane	( Vector2 const &xySize, Vector3 const &centerPos, Rgba const &color = RGBA_WHITE_COLOR, const AABB2 &uvBounds = AABB2::ONE_BY_ONE );
	static Mesh*	CreateCube	( Vector3 const &size, Vector3 const &centerPos = Vector3::ZERO, Rgba const &color = RGBA_WHITE_COLOR, const AABB2& uvTop = AABB2::ONE_BY_ONE, const AABB2& uvSide = AABB2::ONE_BY_ONE, const AABB2& uvBottom = AABB2::ONE_BY_ONE );
	static Mesh*	CreateSphere( float radius, unsigned int wedges, unsigned int slices, Vector3 centerPos = Vector3::ZERO, Rgba const &color = RGBA_WHITE_COLOR );

};