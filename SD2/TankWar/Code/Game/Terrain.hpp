#pragma once
#include "Game/GameObject.hpp"
#include <functional>

typedef std::vector< Renderable* > ChunkList;

enum eTerrainQuadVetrex
{
	TERRAIN_QUAD_BOTTOM_LEFT = 0,
	TERRAIN_QUAD_BOTTOM_RIGHT,
	TERRAIN_QUAD_TOP_LEFT,
	TERRAIN_QUAD_TOP_RIGHT,
	NUM_TERRAIN_QUAD_VERTICES
};

class Terrain : public GameObject
{
public:
	 Terrain( Vector3 spawnPosition, IntVector2 gridSize, float maxHeight, std::string heightMapImagePath );
	~Terrain();

public:
	float			 m_maxHeight			= 2.f;
	IntVector2		 m_sampleSize			= IntVector2( 100, 100 );		// Same as terrain dimension
	ChunkList		 m_chunks;

private:
	Image			*m_heightMapImage		= nullptr;

public:
	void			Update( float deltaSeconds );
	float			GetYCoordinateForMyPositionAt	( Vector2 myXZPosition );
	Vector3			Get3DCoordinateForMyPositionAt	( Vector2 myXZPosition );
	Matrix44		GetModelMatrixForMyPositionAt	( Vector2 myXZPosition, Vector2 uvForwardDirection, Vector2 uvRightDirection );

private:
	// Surface Patch
	Vector3			SinWavePlane( float u, float v );
	Vector3			GetVertexPositionUsingHeightMap( float u, float v );
	Vector3			GiveQuadVertexForMyPositionAt( Vector2 myXZPosition, eTerrainQuadVetrex cornerVertex );

	// Chunking
	ChunkList		MakeChunksUsingSurfacePatch( std::function<Vector3( float, float )> SurfacePatch, IntVector2 maxChunkDimension );

	// Convenience Methods
	inline Vector3	SinWavePlane( Vector2 uv ) { return SinWavePlane( uv.x, uv.y ); }
	inline Vector3	GetVertexPositionUsingHeightMap( Vector2 uv ) { return GetVertexPositionUsingHeightMap( uv.x, uv.y ); }
};