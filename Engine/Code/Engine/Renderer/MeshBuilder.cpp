#pragma once
#include "MeshBuilder.hpp"

void MeshBuilder::Begin( ePrimitiveType primType, bool useIndices )
{
	m_drawInstruction.primitiveType		= primType;
	m_drawInstruction.isUsingIndices	= useIndices;

	if( useIndices )
		m_drawInstruction.startIndex	= (unsigned int)m_indices.size();
	else
		m_drawInstruction.startIndex	= (unsigned int)m_vertices.size();
}

void MeshBuilder::End()
{
	unsigned int endIndex;
	if( m_drawInstruction.isUsingIndices )
		endIndex = (unsigned int)m_indices.size();
	else
		endIndex = (unsigned int)m_vertices.size();

	m_drawInstruction.elementCount = endIndex - m_drawInstruction.startIndex;
}

void MeshBuilder::SetColor( Rgba const &color )
{
	m_stamp.m_color = color;
}

void MeshBuilder::SetUV( Vector2 const &uv )
{
	m_stamp.m_UVs = uv;
}

void MeshBuilder::SetUV( float x, float y )
{
	SetUV( Vector2( x, y ) );
}

unsigned int MeshBuilder::PushVertex( Vector3 const &position )
{
	m_stamp.m_position = position;
	m_vertices.push_back( m_stamp );

	return (unsigned int)m_vertices.size() - 1;
}

void MeshBuilder::AddFace( unsigned int idx1, unsigned int idx2, unsigned int idx3 )
{
	m_indices.push_back( idx1 );
	m_indices.push_back( idx2 );
	m_indices.push_back( idx3 );
}

Mesh* MeshBuilder::ConstructMesh() const
{
	Mesh *mesh = new Mesh();
	mesh->SetIndices				( (unsigned int) m_indices.size(),  m_indices.data() );
	mesh->SetVertices <Vertex_3DPCU>( (unsigned int)m_vertices.size(), m_vertices.data() );
	mesh->SetDrawInstruction( m_drawInstruction );

	return mesh;
}

Mesh* MeshBuilder::CreatePlane( Vector2 xySize, Vector3 centerPos )
{
	Vector3 halfSize = xySize.GetAsVector3() / 2.f;
	Vector3 blCorner = centerPos - halfSize;
	Vector3 trCorner = centerPos + halfSize;
	Vector3 brCorner = Vector3( blCorner.x + xySize.x,	blCorner.y,				blCorner.z );
	Vector3 tlCorner = Vector3( blCorner.x,				blCorner.y + xySize.y,	blCorner.z );

	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );

	// Front Face
	mb.SetUV( 0.f, 0.f );
	unsigned int idx = mb.PushVertex( blCorner );

	mb.SetUV( 1.f, 0.f );
	mb.PushVertex( brCorner );

	mb.SetUV( 1.f, 1.f );
	mb.PushVertex( trCorner );

	mb.SetUV( 0.f, 1.f );
	mb.PushVertex( tlCorner );

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	mb.End();

	return mb.ConstructMesh();
}

Mesh* MeshBuilder::CreateCube( Vector3 size, Vector3 centerPos /* = Vector3::ZERO */ )
{
		
	/*
	      h_________ g			VERTEX[8] ORDER:
		  /|       /|				( a, b, c, d, e, f, g, h )  is,
		 / | top  / |				( 0, 1, 2, 3, 4, 5, 6, 7 )  .
	   e/__|_____/f |			
		|  |_____|__|			   y|   
		| d/     |  /c				|  / z
		| /  bot | /				| /
		|/_______|/					|/______ x
		a         b
	*/

	const Vector3 half_dimensions = size / 2.f;

	// Get coordinates for all vertexes
	const Vector3 bot_center	( centerPos.x,		centerPos.y - half_dimensions.y,		centerPos.z );
	const Vector3 top_center	( centerPos.x,		centerPos.y + half_dimensions.y,		centerPos.z );

	const Vector3 vertexPos[8] = {
		Vector3( bot_center.x - half_dimensions.x, bot_center.y, bot_center.z - half_dimensions.z ),
		Vector3( bot_center.x + half_dimensions.x, bot_center.y, bot_center.z - half_dimensions.z ),
		Vector3( bot_center.x + half_dimensions.x, bot_center.y, bot_center.z + half_dimensions.z ),
		Vector3( bot_center.x - half_dimensions.x, bot_center.y, bot_center.z + half_dimensions.z ),
		Vector3( top_center.x - half_dimensions.x, top_center.y, top_center.z - half_dimensions.z ),
		Vector3( top_center.x + half_dimensions.x, top_center.y, top_center.z - half_dimensions.z ),
		Vector3( top_center.x + half_dimensions.x, top_center.y, top_center.z + half_dimensions.z ),
		Vector3( top_center.x - half_dimensions.x, top_center.y, top_center.z + half_dimensions.z )
	};

	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );

	// Front Face
	// e f
	// a b
	mb.SetUV( 0.f, 0.f );
	unsigned int idx = mb.PushVertex( vertexPos[0] );						// a, 0
	mb.SetUV( 1.f, 0.f );
	mb.PushVertex( vertexPos[1] );											// b, 1
	mb.SetUV( 1.f, 1.f );
	mb.PushVertex( vertexPos[5] );											// f, 5
	mb.SetUV( 0.f, 1.f );
	mb.PushVertex( vertexPos[4] );											// e, 4
													
	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );
	
	// Back Face
	// g h
	// c d
	mb.SetUV( 0.f, 0.f );
	idx = mb.PushVertex( vertexPos[2] );									// c, 2
	mb.SetUV( 1.f, 0.f );
	mb.PushVertex( vertexPos[3] );											// d, 3
	mb.SetUV( 1.f, 1.f );
	mb.PushVertex( vertexPos[7] );											// h, 7
	mb.SetUV( 0.f, 1.f );
	mb.PushVertex( vertexPos[6] );											// g, 6

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	// Left Face
	// h e
	// d a
	mb.SetUV( 0.f, 0.f );
	idx = mb.PushVertex( vertexPos[3] );									// d, 3
	mb.SetUV( 1.f, 0.f );
	mb.PushVertex( vertexPos[0] );											// a, 0
	mb.SetUV( 1.f, 1.f );
	mb.PushVertex( vertexPos[4] );											// e, 4
	mb.SetUV( 0.f, 1.f );
	mb.PushVertex( vertexPos[7] );											// h, 7

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	// Right Face
	// f g
	// b c
	mb.SetUV( 0.f, 0.f );
	idx = mb.PushVertex( vertexPos[1] );									// b, 1
	mb.SetUV( 1.f, 0.f );
	mb.PushVertex( vertexPos[2] );											// c, 2
	mb.SetUV( 1.f, 1.f );
	mb.PushVertex( vertexPos[6] );											// g, 6
	mb.SetUV( 0.f, 1.f );
	mb.PushVertex( vertexPos[5] );											// f, 5

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	// Top Face
	// h g
	// e f
	mb.SetUV( 0.f, 0.f );
	idx = mb.PushVertex( vertexPos[4] );									// e, 4
	mb.SetUV( 1.f, 0.f );
	mb.PushVertex( vertexPos[5] );											// f, 5
	mb.SetUV( 1.f, 1.f );
	mb.PushVertex( vertexPos[6] );											// g, 6
	mb.SetUV( 0.f, 1.f );
	mb.PushVertex( vertexPos[7] );											// h, 7

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );
	
	// Bottom Face
	// a b
	// d c
	mb.SetUV( 0.f, 0.f );
	idx = mb.PushVertex( vertexPos[3] );									// d, 3
	mb.SetUV( 1.f, 0.f );
	mb.PushVertex( vertexPos[2] );											// c, 2
	mb.SetUV( 1.f, 1.f );
	mb.PushVertex( vertexPos[1] );											// b, 1
	mb.SetUV( 0.f, 1.f );
	mb.PushVertex( vertexPos[0] );											// a, 0

	mb.AddFace( idx + 0, idx + 1, idx + 2 );
	mb.AddFace( idx + 2, idx + 3, idx + 0 );

	mb.End();

	return mb.ConstructMesh();
}

Mesh* MeshBuilder::CreateSphere( float radius, unsigned int wedges, unsigned int slices, Vector3 centerPos /* = Vector3::ZERO */ )
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );

	for( unsigned int sliceIdx = 0; sliceIdx <= slices; sliceIdx++ )
	{
		float v			= (float)sliceIdx / (float)slices;
		TODO("Idk why -180, 0 works. 0, 180 gets flipped Sphere. -90, 90 gets half Sphere..");
		float altitude	= RangeMapFloat( v, 0.f, 1.f, -180.f, 0.f );

		for( unsigned int wedgeIdx = 0; wedgeIdx <= wedges; wedgeIdx++ )
		{
			float u				= (float)wedgeIdx / (float)wedges;
			float rotation		= 360.f * u;
			Vector3 position	= centerPos + PolarToCartesian( radius, rotation, altitude );
			
			mb.SetUV( u, v );
			mb.PushVertex( position );
		}
	}

	for( unsigned int sliceIdx = 0; sliceIdx < slices; sliceIdx++ )
	{
		for( unsigned int wedgeIdx = 0; wedgeIdx < wedges; wedgeIdx++ )
		{
			unsigned int bottomLeftIdx	= ( wedges * sliceIdx ) + wedgeIdx;
			unsigned int topLeftIdx		= bottomLeftIdx + wedges;
			unsigned int bottomRightIdx = bottomLeftIdx + 1;
			unsigned int topRightIdx	= bottomRightIdx + wedges;

			mb.AddFace( bottomLeftIdx,	bottomRightIdx, topRightIdx );
			mb.AddFace( topRightIdx,	topLeftIdx,		bottomLeftIdx );
		}
	}

	mb.End();

	return mb.ConstructMesh();
}