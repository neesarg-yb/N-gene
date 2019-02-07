#pragma once
#include "Cube.hpp"

Cube::Cube( Vector3 const &center )
{
	m_wordBounds = AABB3( center, 1.f, 1.f, 1.f );
	m_mesh = Cube::ConstructMesh( center, Vector3::ONE_ALL );
}

Cube::~Cube()
{
	delete m_mesh;
	m_mesh = nullptr;
}

Mesh* Cube::ConstructMesh( Vector3 const &center, Vector3 const &size, AABB2 const &uvSide /* = AABB2::ONE_BY_ONE */, AABB2 const &uvBottom /* = AABB2::ONE_BY_ONE */, AABB2 const &uvTop /* = AABB2::ONE_BY_ONE */, Rgba const &color /* = RGBA_WHITE_COLOR */ )
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );
	/*
	      7_________ 6			VERTEX[8] ORDER:
		  /|       /|				( 0, 1, 2, 3, 4, 5, 6, 7 )
		 / | top  / |				
	   4/__|_____/5 |			
		|  |_____|__|			   z|   
		| 3/     |  /2				|  / x
		| /  bot | /				| /
		|/_______|/			y ______|/ 
		0         1
	*/
	Vector3 const halfDim  = size * 0.5f;
	Vector3 const vertexPos[8] = {
		Vector3( center.x - halfDim.x,	center.y + halfDim.y,	center.z - halfDim.z ),
		Vector3( center.x - halfDim.x,	center.y - halfDim.y,	center.z - halfDim.z ),
		Vector3( center.x + halfDim.x,	center.y - halfDim.y,	center.z - halfDim.z ),
		Vector3( center.x + halfDim.x,	center.y + halfDim.y,	center.z - halfDim.z ),
		Vector3( center.x - halfDim.x,	center.y + halfDim.y,	center.z + halfDim.z ),
		Vector3( center.x - halfDim.x,	center.y - halfDim.y,	center.z + halfDim.z ),
		Vector3( center.x + halfDim.x,	center.y - halfDim.y,	center.z + halfDim.z ),
		Vector3( center.x + halfDim.x,	center.y + halfDim.y,	center.z + halfDim.z )
	};

	// Back Face (towards you)
	// 4 5
	// 0 1
	mb.SetColor( color );
	mb.SetUV( uvSide.mins.x, uvSide.mins.y );
	mb.SetNormal( 0.f, 0.f, -1.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	unsigned int idx = mb.PushVertex( vertexPos[0] );						// 0
	mb.SetColor( color );
	mb.SetUV( uvSide.maxs.x, uvSide.mins.y );
	mb.SetNormal( 0.f, 0.f, -1.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[1] );											// 1
	mb.SetColor( color );
	mb.SetUV( uvSide.maxs.x, uvSide.maxs.y );
	mb.SetNormal( 0.f, 0.f, -1.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[5] );											// 5
	mb.SetColor( color );
	mb.SetUV( uvSide.mins.x, uvSide.maxs.y );
	mb.SetNormal( 0.f, 0.f, -1.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[4] );											// 4

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	// Front Face (away from you)
	// 6 7
	// 2 3
	mb.SetColor( color );
	mb.SetUV( uvSide.mins.x, uvSide.mins.y );
	mb.SetNormal( 0.f, 0.f, 1.f );
	mb.SetTangent4( -1.f, 0.f, 0.f, 1.f );
	idx = mb.PushVertex( vertexPos[2] );									// 2
	mb.SetColor( color );
	mb.SetUV( uvSide.maxs.x, uvSide.mins.y );
	mb.SetNormal( 0.f, 0.f, 1.f );
	mb.SetTangent4( -1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[3] );											// 3
	mb.SetColor( color );
	mb.SetUV( uvSide.maxs.x, uvSide.maxs.y );
	mb.SetNormal( 0.f, 0.f, 1.f );
	mb.SetTangent4( -1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[7] );											// 7
	mb.SetColor( color );
	mb.SetUV( uvSide.mins.x, uvSide.maxs.y );
	mb.SetNormal( 0.f, 0.f, 1.f );
	mb.SetTangent4( -1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[6] );											// 6

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	// Left Face
	// 7 4
	// 3 0
	mb.SetColor( color );
	mb.SetUV( uvSide.mins.x, uvSide.mins.y );
	mb.SetNormal( -1.f, 0.f, 0.f );
	mb.SetTangent4( 0.f, 0.f, -1.f, 1.f );
	idx = mb.PushVertex( vertexPos[3] );									// 3
	mb.SetColor( color );
	mb.SetUV( uvSide.maxs.x, uvSide.mins.y );
	mb.SetNormal( -1.f, 0.f, 0.f );
	mb.SetTangent4( 0.f, 0.f, -1.f, 1.f );
	mb.PushVertex( vertexPos[0] );											// 0
	mb.SetColor( color );
	mb.SetUV( uvSide.maxs.x, uvSide.maxs.y );
	mb.SetNormal( -1.f, 0.f, 0.f );
	mb.SetTangent4( 0.f, 0.f, -1.f, 1.f );
	mb.PushVertex( vertexPos[4] );											// 4
	mb.SetColor( color );
	mb.SetUV( uvSide.mins.x, uvSide.maxs.y );
	mb.SetNormal( -1.f, 0.f, 0.f );
	mb.SetTangent4( 0.f, 0.f, -1.f, 1.f );
	mb.PushVertex( vertexPos[7] );											// 7

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	// Right Face
	// 5 6
	// 1 2
	mb.SetColor( color );
	mb.SetUV( uvSide.mins.x, uvSide.mins.y );
	mb.SetNormal( 1.f, 0.f, 0.f );
	mb.SetTangent4( 0.f, 0.f, 1.f, 1.f );
	idx = mb.PushVertex( vertexPos[1] );									// 1
	mb.SetColor( color );
	mb.SetUV( uvSide.maxs.x, uvSide.mins.y );
	mb.SetNormal( 1.f, 0.f, 0.f );
	mb.SetTangent4( 0.f, 0.f, 1.f, 1.f );
	mb.PushVertex( vertexPos[2] );											// 2
	mb.SetColor( color );
	mb.SetUV( uvSide.maxs.x, uvSide.maxs.y );
	mb.SetNormal( 1.f, 0.f, 0.f );
	mb.SetTangent4( 0.f, 0.f, 1.f, 1.f );
	mb.PushVertex( vertexPos[6] );											// 6
	mb.SetColor( color );
	mb.SetUV( uvSide.mins.x, uvSide.maxs.y );
	mb.SetNormal( 1.f, 0.f, 0.f );
	mb.SetTangent4( 0.f, 0.f, 1.f, 1.f );
	mb.PushVertex( vertexPos[5] );											// 5

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	// Top Face
	// 7 6
	// 4 5
	mb.SetColor( color );
	mb.SetUV( uvTop.mins.x, uvTop.mins.y );
	mb.SetNormal( 0.f, 1.f, 0.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	idx = mb.PushVertex( vertexPos[4] );									// 4
	mb.SetColor( color );
	mb.SetUV( uvTop.maxs.x, uvTop.mins.y );
	mb.SetNormal( 0.f, 1.f, 0.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[5] );											// 5
	mb.SetColor( color );
	mb.SetUV( uvTop.maxs.x, uvTop.maxs.y );
	mb.SetNormal( 0.f, 1.f, 0.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[6] );											// 6
	mb.SetColor( color );
	mb.SetUV( uvTop.mins.x, uvTop.maxs.y );
	mb.SetNormal( 0.f, 1.f, 0.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[7] );											// 7

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	// Bottom Face
	// 0 1
	// 3 2
	mb.SetColor( color );
	mb.SetUV( uvBottom.mins.x, uvBottom.mins.y );
	mb.SetNormal( 0.f, -1.f, 0.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	idx = mb.PushVertex( vertexPos[3] );									// 3
	mb.SetColor( color );
	mb.SetUV( uvBottom.maxs.x, uvBottom.mins.y );
	mb.SetNormal( 0.f, -1.f, 0.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[2] );											// 2
	mb.SetColor( color );
	mb.SetUV( uvBottom.maxs.x, uvBottom.maxs.y );
	mb.SetNormal( 0.f, -1.f, 0.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[1] );											// 1
	mb.SetColor( color );
	mb.SetUV( uvBottom.mins.x, uvBottom.maxs.y );
	mb.SetNormal( 0.f, -1.f, 0.f );
	mb.SetTangent4( 1.f, 0.f, 0.f, 1.f );
	mb.PushVertex( vertexPos[0] );											// 0

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	mb.End();

	return mb.ConstructMesh<Vertex_Lit>();
}

