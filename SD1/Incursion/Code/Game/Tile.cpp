#include "Tile.hpp"


Tile::Tile()
{
	m_tileCoords = IntVector2( 0, 0 );
	m_tileType = TILE_TYPE_GRASS;
}

Tile::Tile( IntVector2 tileCoords, TileType typeOfTile ) 
{
	m_tileCoords = tileCoords;
	m_tileType = typeOfTile;
}

Tile::~Tile()
{

}

AABB2 Tile::GetBoundsOfTile() {
	AABB2 bounds = AABB2( (float)m_tileCoords.x , (float)m_tileCoords.y , (float)m_tileCoords.x + 1.f, (float)m_tileCoords.y + 1.f );

	return bounds;
}

Vector2  Tile::GetCenterOfTile() {
	Vector2 center = Vector2( m_tileCoords.x + 0.5f , m_tileCoords.y + 0.5f );

	return center;
}

Vector2 Tile::GetClosestPointOnTileFrom( Vector2 outSidePoint ) {
	Vector2 temp;
	temp.x = MathUtil::ClampFloat( outSidePoint.x , (float)m_tileCoords.x, (float)m_tileCoords.x + 1.f);
	temp.y = MathUtil::ClampFloat( outSidePoint.y , (float)m_tileCoords.y, (float)m_tileCoords.y + 1.f);

	return temp;
}