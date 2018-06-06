#include "TileDefination.hpp"

TileDefination TileDefination::s_TileDefinations[ NUM_TILE_TYPES ];

TileDefination::TileDefination( TileType typeIndex, bool isSolid, const IntVector2& tileCoords ) 
{
	m_tileType = typeIndex;
	m_isSolid = isSolid;
	m_tileCoords = tileCoords;
}

TileDefination::TileDefination() {

}

TileDefination::~TileDefination()
{

}

AABB2 TileDefination::GetBounds() {
	AABB2 bounds = AABB2( (float)m_tileCoords.x, (float)m_tileCoords.y, (float)m_tileCoords.x + 1.f, (float)m_tileCoords.y + 1.f );

	return bounds;
}