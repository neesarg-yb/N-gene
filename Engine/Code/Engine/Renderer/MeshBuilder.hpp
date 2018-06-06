#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Mesh.hpp"

class MeshBuilder
{
public:
	Vertex_3DPCU					m_stamp;
	std::vector< Vertex_3DPCU >		m_vertices;
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
	unsigned int	PushVertex	( Vector3 const &position );	// Pushes the stampVertex_3DPCU, returns the index of Vertex

	// Set the Indices
	void	AddFace( unsigned int idx1, unsigned int idx2, unsigned int idx3 );

	// Create Mesh with data stored..
	Mesh*	ConstructMesh() const;										// Call End() before CreateMesh
	
public:
	static Mesh* CreatePlane ( Vector2 xySize, Vector3 centerPos );
	static Mesh* CreateCube	 ( Vector3 size, Vector3 centerPos = Vector3::ZERO );
	static Mesh* CreateSphere( float radius, unsigned int wedges, unsigned int slices, Vector3 centerPos = Vector3::ZERO );
};