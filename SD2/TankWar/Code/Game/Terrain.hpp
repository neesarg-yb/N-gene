#pragma once
#include "Game/GameObject.hpp"

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
	 Terrain( Vector3 spawnPosition, uint gridSize, float maxHeight );
	~Terrain();

public:
	float			m_maxHeight			= 2.f;
	uint			m_sampleSize		= 100U;
	AABB2			m_terrainBoundsXZ	= AABB2( -100.f, -100.f, 100.f, 100.f );

public:
	void			Update( float deltaSeconds );
	float			GetYCoordinateForMyPositionAt( Vector2 myXZPosition );

private:
	Vector3			SinWavePlane( float u, float v );
	Vector3			GiveQuadVertexForMyPositionAt( Vector2 myXZPosition, eTerrainQuadVetrex cornerVertex );

	// Convenience Methods
	inline Vector3	SinWavePlane( Vector2 uv ) { return SinWavePlane( uv.x, uv.y ); };
};