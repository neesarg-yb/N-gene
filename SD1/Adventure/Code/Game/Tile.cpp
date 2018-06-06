#include "Tile.hpp"


Tile::Tile( IntVector2 tileCoords, std::string tileDef )
{
	m_tileCoords = tileCoords;

	std::map< std::string, TileDefinition* >::iterator it = TileDefinition::s_definitions.find( tileDef );
	GUARANTEE_OR_DIE( it != TileDefinition::s_definitions.end() , "TileDefination " + tileDef + " not found..!" );

	m_tileDef = it->second;
	m_tileExtraInfo = new TileExtraInfo( m_tileDef->m_startTags );
}

Tile::~Tile()
{

}

void Tile::ChangeTileDef( TileDefinition& tileDef )
{
	m_tileDef = &tileDef;

	delete m_tileExtraInfo;
	m_tileExtraInfo = new TileExtraInfo( tileDef.m_startTags );
}

AABB2 Tile::GetWorldBounds()
{
	AABB2	boundsOfTile = AABB2( (float)m_tileCoords.x, (float)m_tileCoords.y, (float)m_tileCoords.x + 1.f, (float)m_tileCoords.y + 1.f );
	return	boundsOfTile;
}

Vector2 Tile::GetClosestPointOnTileFrom( Vector2 outSidePoint ) {
	Vector2 temp;
	temp.x = ClampFloat( outSidePoint.x , (float)m_tileCoords.x, (float)m_tileCoords.x + 1.f);
	temp.y = ClampFloat( outSidePoint.y , (float)m_tileCoords.y, (float)m_tileCoords.y + 1.f);

	return temp;
}