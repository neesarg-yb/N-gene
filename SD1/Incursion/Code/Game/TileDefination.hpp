#pragma once

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/AABB2.hpp"

enum TileType {
	TILE_TYPE_GRASS,
	TILE_TYPE_STONE,
	NUM_TILE_TYPES
};

class TileDefination
{
public:
	 TileDefination();
	 TileDefination( TileType typeIndex, bool isSolid, const IntVector2& tileCoords );
	~TileDefination();

	static TileDefination s_TileDefinations[ NUM_TILE_TYPES ];

	bool		m_isSolid;
	TileType	m_tileType;
	IntVector2	m_tileCoords;

	AABB2 GetBounds();

private:
};