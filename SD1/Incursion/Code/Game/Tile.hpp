#pragma once

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Game/TileDefination.hpp"
#include <math.h>

class Tile
{
public:
	IntVector2		m_tileCoords;
	TileType		m_tileType;

	 Tile();
	 Tile( IntVector2 tileCoords, TileType typeOfTile );
	~Tile();
	
	AABB2	GetBoundsOfTile();
	Vector2 GetCenterOfTile();
	Vector2 GetClosestPointOnTileFrom( Vector2 outSidePoint );

private:

};