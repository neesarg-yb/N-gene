#pragma once
#include <functional>
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Game/Potential Engine/GameObject.hpp"

class Scene;
typedef std::vector< Renderable* > ChunkList;

enum eTerrainQuadVetrex
{
	TERRAIN_QUAD_BOTTOM_LEFT = 0,
	TERRAIN_QUAD_BOTTOM_RIGHT,
	TERRAIN_QUAD_TOP_LEFT,
	TERRAIN_QUAD_TOP_RIGHT,
	NUM_TERRAIN_QUAD_VERTICES
};

enum eTerrainMaterial
{
	TERRAIN_GRIDLINES,
	TERRAIN_GRASS
};

class Terrain : public GameObject
{
public:
	 // Terrain is always generated with 1x1 squares
	 // Texture used to render the terrain gets repeated on each of these 1x1 square
	 Terrain( Vector3 spawnPosition, IntVector2 gridSize, float maxHeight, std::string heightMapImagePath, eTerrainMaterial materialFile );
	~Terrain();

public:
	float			 m_maxHeight			= 2.f;
	AABB3			 m_worldBounds;
	IntVector2		 m_sampleSize			= IntVector2( 100, 100 );		// Same as terrain dimension
	ChunkList		 m_chunks;

private:
	Image			*m_heightMapImage		= nullptr;
	std::string		 m_materialFilePath		= "";

public:
	void			Update( float deltaSeconds );
	void			AddRenderablesToScene( Scene &activeScene );
	void			RemoveRenderablesFromScene( Scene &activeScene );

	// Fetching the Position
	Vector3			GetNormalForMyPositionAt		( Vector2 myXZPosition ) const;
	float			GetYCoordinateForMyPositionAt	( Vector2 myXZPosition, float yOffset = 0.f ) const;
	Vector3			Get3DCoordinateForMyPositionAt	( Vector2 myXZPosition, float yOffset = 0.f ) const;
	Plane3			GetPlaneForMyPositionAt			( Vector2 myXZPosition ) const; // Returns most accurate representation of terrain-plane at that point

	// Raycast
	RaycastResult	Raycast( Vector3 const &startPosition, Vector3 direction, float const maxDistance, float const accuracy );

	// Collision
	Vector3			CheckCollisionWithSphere( Vector3 const &center, float radius, bool &outIsColliding ) const;

private:
	// Surface Patch
	Vector3			SinWavePlane( float u, float v );
	Vector3			GetVertexPositionUsingHeightMap( float u, float v ) const;
	Vector3			GiveQuadVertexForMyPositionAt( Vector2 myXZPosition, eTerrainQuadVetrex cornerVertex ) const;

	// Chunking
	ChunkList		MakeChunksUsingSurfacePatch( std::function<Vector3( float, float )> SurfacePatch, IntVector2 maxChunkDimension );

	// Convenience Methods
	inline Vector3	SinWavePlane( Vector2 uv ) { return SinWavePlane( uv.x, uv.y ); }

public:
	inline float	GetYCoordinateForMyPositionAt	( float x, float z, float yOffset = 0.f )	const { return GetYCoordinateForMyPositionAt( Vector2( x, z ), yOffset ); }
	inline Vector3	Get3DCoordinateForMyPositionAt	( float x, float z, float yOffset = 0.f )	const { return Get3DCoordinateForMyPositionAt( Vector2( x, z ), yOffset ); }
	inline Plane3	GetPlaneForMyPositionAt			( float x, float z )						const { return GetPlaneForMyPositionAt( Vector2(x, z) ); }

private:
	inline Vector3	GetVertexPositionUsingHeightMap( Vector2 uv ) const { return GetVertexPositionUsingHeightMap( uv.x, uv.y ); }
};