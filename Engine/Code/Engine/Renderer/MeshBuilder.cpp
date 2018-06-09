#pragma once
#include "MeshBuilder.hpp"

void MeshBuilder::Begin( ePrimitiveType primType, bool useIndices )
{
	m_drawInstruction.primitiveType		= primType;
	m_drawInstruction.isUsingIndices	= useIndices;

	if( useIndices )
		m_drawInstruction.startIndex	= 0;
	else
		m_drawInstruction.startIndex	= 0;
}

void MeshBuilder::End()
{
	unsigned int endIndex;
	if( m_drawInstruction.isUsingIndices )
		endIndex = (unsigned int)m_indices.size();
	else
		endIndex = (unsigned int)m_vertices.size();

	m_drawInstruction.elementCount = endIndex;
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

void MeshBuilder::SetNormal(Vector3 const &normal)
{
	m_stamp.m_normal = normal;
}

void MeshBuilder::SetNormal(float x, float y, float z)
{
	m_stamp.m_normal = Vector3( x, y, z );
}

void MeshBuilder::SetTangent4( Vector4 const &tangent4 )
{
	m_stamp.m_tangent = tangent4;
}

void MeshBuilder::SetTangent4( float x, float y, float z, float w )
{
	m_stamp.m_tangent = Vector4( x, y, z, w );
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

void MeshBuilder::AddPlane( Vector2 const &xySize, Vector3 const &centerPos, Rgba const &color /* = RGBA_WHITE_COLOR */, const AABB2 &uvBounds /* = AABB2::ONE_BY_ONE */ )
{
	Vector3 halfSize = xySize.GetAsVector3() / 2.f;
	Vector3 blCorner = centerPos - halfSize;
	Vector3 trCorner = centerPos + halfSize;
	Vector3 brCorner = Vector3( blCorner.x + xySize.x,	blCorner.y,				blCorner.z );
	Vector3 tlCorner = Vector3( blCorner.x,				blCorner.y + xySize.y,	blCorner.z );

	// If parameters doesn't match with current operation
	bool mbParameterMatches = true;
	if( this->m_drawInstruction.isUsingIndices != true )
		mbParameterMatches = false;
	else if( this->m_drawInstruction.primitiveType != PRIMITIVE_TRIANGES )
		mbParameterMatches = false;
	// Die
	GUARANTEE_OR_DIE( mbParameterMatches, "Meshbuilder: drawInstruction parameters isUsingIndices or primitiveType doesn't match with current operation!" );

	this->Begin( PRIMITIVE_TRIANGES, true );

	// Back Face (towards you)
	this->SetColor( color );
	this->SetUV( uvBounds.mins.x, uvBounds.mins.y );
	this->SetNormal( 0.f, 0.f, -1.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	unsigned int idx = this->PushVertex( blCorner );
	
	this->SetColor( color );
	this->SetUV( uvBounds.maxs.x, uvBounds.mins.y );
	this->SetNormal( 0.f, 0.f, -1.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( brCorner );
	
	this->SetColor( color );
	this->SetUV( uvBounds.maxs.x, uvBounds.maxs.y );
	this->SetNormal( 0.f, 0.f, -1.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( trCorner );
	
	this->SetColor( color );
	this->SetUV( uvBounds.mins.x, uvBounds.maxs.y );
	this->SetNormal( 0.f, 0.f, -1.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( tlCorner );

	this->AddFace( idx + 0, idx + 1, idx + 2 );
	this->AddFace( idx + 2, idx + 3, idx + 0 );

	this->End();
}

void MeshBuilder::AddCube( Vector3 const &size, Vector3 const &centerPos /* = Vector3::ZERO */, Rgba const &color /* = RGBA_WHITE_COLOR */, const AABB2& uvTop /* = AABB2::ONE_BY_ONE */, const AABB2& uvSide /* = AABB2::ONE_BY_ONE */, const AABB2& uvBottom /* = AABB2::ONE_BY_ONE */ )
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

	// If parameters doesn't match with current operation
	bool mbParameterMatches = true;
	if( this->m_drawInstruction.isUsingIndices != true )
		mbParameterMatches = false;
	else if( this->m_drawInstruction.primitiveType != PRIMITIVE_TRIANGES )
		mbParameterMatches = false;
	// Die
	GUARANTEE_OR_DIE( mbParameterMatches, "Meshbuilder: drawInstruction parameters isUsingIndices or primitiveType doesn't match with current operation!" );

	this->Begin( PRIMITIVE_TRIANGES, true );

	// Back Face (towards you)
	// e f
	// a b
	this->SetColor( color );
	this->SetUV( uvSide.mins.x, uvSide.mins.y );
	this->SetNormal( 0.f, 0.f, -1.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	unsigned int idx = this->PushVertex( vertexPos[0] );						// a, 0
	this->SetColor( color );
	this->SetUV( uvSide.maxs.x, uvSide.mins.y );
	this->SetNormal( 0.f, 0.f, -1.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[1] );											// b, 1
	this->SetColor( color );
	this->SetUV( uvSide.maxs.x, uvSide.maxs.y );
	this->SetNormal( 0.f, 0.f, -1.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[5] );											// f, 5
	this->SetColor( color );
	this->SetUV( uvSide.mins.x, uvSide.maxs.y );
	this->SetNormal( 0.f, 0.f, -1.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[4] );											// e, 4
													
	this->AddFace( idx + 0, idx + 1, idx + 2 );
	this->AddFace( idx + 2, idx + 3, idx + 0 );
	
	// Front Face (away from you)
	// g h
	// c d
	this->SetColor( color );
	this->SetUV( uvSide.mins.x, uvSide.mins.y );
	this->SetNormal( 0.f, 0.f, 1.f );
	this->SetTangent4( -1.f, 0.f, 0.f, 1.f );
	idx = this->PushVertex( vertexPos[2] );									// c, 2
	this->SetColor( color );
	this->SetUV( uvSide.maxs.x, uvSide.mins.y );
	this->SetNormal( 0.f, 0.f, 1.f );
	this->SetTangent4( -1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[3] );											// d, 3
	this->SetColor( color );
	this->SetUV( uvSide.maxs.x, uvSide.maxs.y );
	this->SetNormal( 0.f, 0.f, 1.f );
	this->SetTangent4( -1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[7] );											// h, 7
	this->SetColor( color );
	this->SetUV( uvSide.mins.x, uvSide.maxs.y );
	this->SetNormal( 0.f, 0.f, 1.f );
	this->SetTangent4( -1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[6] );											// g, 6

	this->AddFace( idx + 0, idx + 1, idx + 2 );
	this->AddFace( idx + 2, idx + 3, idx + 0 );

	// Left Face
	// h e
	// d a
	this->SetColor( color );
	this->SetUV( uvSide.mins.x, uvSide.mins.y );
	this->SetNormal( -1.f, 0.f, 0.f );
	this->SetTangent4( 0.f, 0.f, -1.f, 1.f );
	idx = this->PushVertex( vertexPos[3] );									// d, 3
	this->SetColor( color );
	this->SetUV( uvSide.maxs.x, uvSide.mins.y );
	this->SetNormal( -1.f, 0.f, 0.f );
	this->SetTangent4( 0.f, 0.f, -1.f, 1.f );
	this->PushVertex( vertexPos[0] );											// a, 0
	this->SetColor( color );
	this->SetUV( uvSide.maxs.x, uvSide.maxs.y );
	this->SetNormal( -1.f, 0.f, 0.f );
	this->SetTangent4( 0.f, 0.f, -1.f, 1.f );
	this->PushVertex( vertexPos[4] );											// e, 4
	this->SetColor( color );
	this->SetUV( uvSide.mins.x, uvSide.maxs.y );
	this->SetNormal( -1.f, 0.f, 0.f );
	this->SetTangent4( 0.f, 0.f, -1.f, 1.f );
	this->PushVertex( vertexPos[7] );											// h, 7

	this->AddFace( idx + 0, idx + 1, idx + 2 );
	this->AddFace( idx + 2, idx + 3, idx + 0 );

	// Right Face
	// f g
	// b c
	this->SetColor( color );
	this->SetUV( uvSide.mins.x, uvSide.mins.y );
	this->SetNormal( 1.f, 0.f, 0.f );
	this->SetTangent4( 0.f, 0.f, 1.f, 1.f );
	idx = this->PushVertex( vertexPos[1] );									// b, 1
	this->SetColor( color );
	this->SetUV( uvSide.maxs.x, uvSide.mins.y );
	this->SetNormal( 1.f, 0.f, 0.f );
	this->SetTangent4( 0.f, 0.f, 1.f, 1.f );
	this->PushVertex( vertexPos[2] );											// c, 2
	this->SetColor( color );
	this->SetUV( uvSide.maxs.x, uvSide.maxs.y );
	this->SetNormal( 1.f, 0.f, 0.f );
	this->SetTangent4( 0.f, 0.f, 1.f, 1.f );
	this->PushVertex( vertexPos[6] );											// g, 6
	this->SetColor( color );
	this->SetUV( uvSide.mins.x, uvSide.maxs.y );
	this->SetNormal( 1.f, 0.f, 0.f );
	this->SetTangent4( 0.f, 0.f, 1.f, 1.f );
	this->PushVertex( vertexPos[5] );											// f, 5

	this->AddFace( idx + 0, idx + 1, idx + 2 );
	this->AddFace( idx + 2, idx + 3, idx + 0 );

	// Top Face
	// h g
	// e f
	this->SetColor( color );
	this->SetUV( uvTop.mins.x, uvTop.mins.y );
	this->SetNormal( 0.f, 1.f, 0.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	idx = this->PushVertex( vertexPos[4] );									// e, 4
	this->SetColor( color );
	this->SetUV( uvTop.maxs.x, uvTop.mins.y );
	this->SetNormal( 0.f, 1.f, 0.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[5] );											// f, 5
	this->SetColor( color );
	this->SetUV( uvTop.maxs.x, uvTop.maxs.y );
	this->SetNormal( 0.f, 1.f, 0.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[6] );											// g, 6
	this->SetColor( color );
	this->SetUV( uvTop.mins.x, uvTop.maxs.y );
	this->SetNormal( 0.f, 1.f, 0.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[7] );											// h, 7

	this->AddFace( idx + 0, idx + 1, idx + 2 );
	this->AddFace( idx + 2, idx + 3, idx + 0 );
	
	// Bottom Face
	// a b
	// d c
	this->SetColor( color );
	this->SetUV( uvBottom.mins.x, uvBottom.mins.y );
	this->SetNormal( 0.f, -1.f, 0.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	idx = this->PushVertex( vertexPos[3] );									// d, 3
	this->SetColor( color );
	this->SetUV( uvBottom.maxs.x, uvBottom.mins.y );
	this->SetNormal( 0.f, -1.f, 0.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[2] );											// c, 2
	this->SetColor( color );
	this->SetUV( uvBottom.maxs.x, uvBottom.maxs.y );
	this->SetNormal( 0.f, -1.f, 0.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[1] );											// b, 1
	this->SetColor( color );
	this->SetUV( uvBottom.mins.x, uvBottom.maxs.y );
	this->SetNormal( 0.f, -1.f, 0.f );
	this->SetTangent4( 1.f, 0.f, 0.f, 1.f );
	this->PushVertex( vertexPos[0] );											// a, 0

	this->AddFace( idx + 0, idx + 1, idx + 2 );
	this->AddFace( idx + 2, idx + 3, idx + 0 );

	this->End();
}

void MeshBuilder::AddSphere( float radius, unsigned int wedges, unsigned int slices, Vector3 centerPos /* = Vector3::ZERO */, Rgba const &color /* = RGBA_WHITE_COLOR */ )
{
	// If parameters doesn't match with current operation
	bool mbParameterMatches = true;
	if( this->m_drawInstruction.isUsingIndices != true )
		mbParameterMatches = false;
	else if( this->m_drawInstruction.primitiveType != PRIMITIVE_TRIANGES )
		mbParameterMatches = false;
	// Die
	GUARANTEE_OR_DIE( mbParameterMatches, "Meshbuilder: drawInstruction parameters isUsingIndices or primitiveType doesn't match with current operation!" );
	
	this->Begin( PRIMITIVE_TRIANGES, true );

	for( unsigned int sliceIdx = 0; sliceIdx <= slices; sliceIdx++ )
	{
		float v			= (float)sliceIdx / (float)slices;
		float altitude	= RangeMapFloat( v, 0.f, 1.f, 180.f, 0.f );

		for( unsigned int wedgeIdx = 0; wedgeIdx <= wedges; wedgeIdx++ )
		{
			float u				= (float)wedgeIdx / (float)wedges;
			float rotation		= 360.f * u;
			Vector3 position	= centerPos + PolarToCartesian( radius, rotation, altitude );
			Vector3 normal		= ( position - centerPos ).GetNormalized();

			// derivative of (tan = d/du Polar)
			float altitude90	= RangeMapFloat( altitude, 180.f, 0.f, -90.f, 90.f );
			float tx			= -1.f * CosDegree( altitude90 ) * SinDegree( rotation ) * radius;
			float ty			= 0.f;
			float tz			= CosDegree( altitude90 ) * CosDegree( rotation ) * radius;
			Vector4 tangent		= Vector4( tx, ty, tz, 1.f ).GetNormalized();

			this->SetUV( u, v );
			this->SetNormal( normal );
			this->SetTangent4( tangent );
			this->SetColor( color );
			this->PushVertex( position );
		}
	}

	for( unsigned int sliceIdx = 0; sliceIdx < slices; sliceIdx++ )
	{
		for( unsigned int wedgeIdx = 0; wedgeIdx < wedges; wedgeIdx++ )
		{
			unsigned int bottomLeftIdx	= ( ( wedges + 1 ) * sliceIdx ) + wedgeIdx;
			unsigned int topLeftIdx		= bottomLeftIdx + wedges + 1;
			unsigned int bottomRightIdx = bottomLeftIdx + 1;
			unsigned int topRightIdx	= topLeftIdx + 1;

			this->AddFace( bottomLeftIdx,	bottomRightIdx, topRightIdx );
			this->AddFace( topRightIdx,		topLeftIdx,		bottomLeftIdx );
		}
	}

	this->End();
}

void MeshBuilder::AddMeshFromSurfacePatch( std::function<Vector3( float, float )> SurfacePatch, Vector2 uvRangeMin, Vector2 uvRangeMax, IntVector2 sampleFrequency, Rgba const &color /* = RGBA_WHITE_COLOR */ )
{
	// If parameters doesn't match with current operation
	bool mbParameterMatches = true;
	if( this->m_drawInstruction.isUsingIndices != true )
		mbParameterMatches = false;
	else if( this->m_drawInstruction.primitiveType != PRIMITIVE_TRIANGES )
		mbParameterMatches = false;
	// Die
	GUARANTEE_OR_DIE( mbParameterMatches, "Meshbuilder: drawInstruction parameters isUsingIndices or primitiveType doesn't match with current operation!" );

	
	Vector2 step;
	step.x = ( uvRangeMax.x - uvRangeMin.x ) / sampleFrequency.x;
	step.y = ( uvRangeMax.y - uvRangeMin.y ) / sampleFrequency.y;

	for( float v = uvRangeMin.y; v <= uvRangeMax.y; v += step.y )
	{
		for( float u = uvRangeMin.x; u <= uvRangeMax.x; u += step.x )
		{
			Vector3 position			= SurfacePatch( u, v );

			Vector3 positionTowardU		= SurfacePatch( u + step.x, v );
			Vector3 positionTowardV		= SurfacePatch( u, v + step.y );
			Vector3 directionTowardU	= positionTowardU - position;
			Vector3 directionTowardV	= positionTowardV - position;
			Vector3 normal				= Vector3::CrossProduct( directionTowardV, directionTowardU ).GetNormalized();

			this->SetUV( u, v );
			this->SetNormal( normal );
			this->SetTangent4( Vector4( directionTowardU.GetNormalized(), 1.f) );
			this->SetColor( color );
			this->PushVertex( position );
		}
	}

	for( int vIdx = 0; vIdx < sampleFrequency.y; vIdx++ )
	{
		for( int uIdx = 0; uIdx < sampleFrequency.x; uIdx++ )
		{
			int bottomLeftIdx	= ( ( sampleFrequency.x + 1 ) * vIdx ) + uIdx;
			int topLeftIdx		= bottomLeftIdx + sampleFrequency.x + 1;
			int bottomRightIdx	= bottomLeftIdx + 1;
			int topRightIdx		= topLeftIdx + 1;

			this->AddFace( bottomLeftIdx,	bottomRightIdx, topRightIdx );
			this->AddFace( topRightIdx,		topLeftIdx,		bottomLeftIdx );
		}
	}

	this->End();
}

void MeshBuilder::SetVertexPositionsRelativeTo( Vector3 pivotPosition )
{
	// For all vertices
		// Assuming that every positions were set relative to Vector3::ZERO
		// Subtract the pivotPosition

	for each (Vertex_Master vert in m_vertices)
		vert.m_position -= pivotPosition;
}

Mesh* MeshBuilder::CreatePlane( Vector2 const &xySize, Vector3 const &centerPos, Rgba const &color /*= RGBA_WHITE_COLOR*/, const AABB2 &uvBounds /*= AABB2::ONE_BY_ONE */ )
{
	MeshBuilder mb;
	mb.AddPlane( xySize, centerPos, color, uvBounds );

	return mb.ConstructMesh <Vertex_Lit>();
}

Mesh* MeshBuilder::CreateCube( Vector3 const &size, Vector3 const &centerPos /*= Vector3::ZERO*/, Rgba const &color /*= RGBA_WHITE_COLOR*/, const AABB2& uvTop /*= AABB2::ONE_BY_ONE*/, const AABB2& uvSide /*= AABB2::ONE_BY_ONE*/, const AABB2& uvBottom /*= AABB2::ONE_BY_ONE */ )
{
	MeshBuilder mb;
	mb.AddCube( size, centerPos, color, uvTop, uvSide, uvBottom );

	return mb.ConstructMesh <Vertex_Lit>();
}

Mesh* MeshBuilder::CreateSphere( float radius, unsigned int wedges, unsigned int slices, Vector3 centerPos /*= Vector3::ZERO*/, Rgba const &color /*= RGBA_WHITE_COLOR */ )
{
	MeshBuilder mb;
	mb.AddSphere( radius, wedges, slices, centerPos, color );

	return mb.ConstructMesh <Vertex_Lit>();
}
