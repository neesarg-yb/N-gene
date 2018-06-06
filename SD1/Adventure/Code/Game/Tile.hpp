#pragma once
#include <string>
#include <map>
#include "Engine/Math/IntVector2.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Game/TileExtraInfo.hpp"

class Tile
{
public:
	 Tile( IntVector2 tileCoords, std::string tileDef );
	~Tile();

	IntVector2			m_tileCoords;
	TileDefinition*		m_tileDef		= nullptr;
	TileExtraInfo*		m_tileExtraInfo = nullptr;

	// Methods for,
	//	 (1) Changing the Tile Type
	//	 (2) Getting world bounds
	void	ChangeTileDef( TileDefinition& tileDef );			// Note: It will replace whole m_tileExtraInfo, as well!!
	AABB2	GetWorldBounds();
	Vector2 GetClosestPointOnTileFrom( Vector2 outSidePoint );

private:

};