#pragma once
#include "BlockDefinition.hpp"

Material*			BlockDefinition::s_material		= nullptr;
SpriteSheet*		BlockDefinition::s_spriteSheet	= nullptr;
BlockDefinition		BlockDefinition::s_definitions[ NUM_BLOCK_TYPES ];

BlockDefinition::BlockDefinition()
{

}

BlockDefinition::BlockDefinition( eBlockType type, bool isSolid, bool isFullyOpaque, bool isNeverVisible, int indoorLightLevel, IntVector2 const &sideTileCoord_TL, IntVector2 const &botTileCoord_TL, IntVector2 const &topTileCoord_TL )
	: m_type( type )
	, m_isSolid( isSolid )
	, m_isFullyOpaque( isFullyOpaque )
	, m_isNeverVisible( isNeverVisible )
	, m_indoorLightLevel( indoorLightLevel )
{
	IntVector2 numTiles			= s_spriteSheet->GetSpriteLayout();
	IntVector2 sideCoord_BL		= IntVector2( sideTileCoord_TL.x,	(numTiles.y - 1) - sideTileCoord_TL.y );	// From top-left to bottom-left
	IntVector2 bottomCoord_BL	= IntVector2( botTileCoord_TL.x,	(numTiles.y - 1) - botTileCoord_TL.y );		// From top-left to bottom-left
	IntVector2 topCoord_BL		= IntVector2( topTileCoord_TL.x,	(numTiles.y - 1) - topTileCoord_TL.y );		// From top-left to bottom-left

	m_uvSide	= s_spriteSheet->GetTexCoordsForSpriteCoords( sideCoord_BL );
	m_uvBottom	= s_spriteSheet->GetTexCoordsForSpriteCoords( bottomCoord_BL );
	m_uvTop		= s_spriteSheet->GetTexCoordsForSpriteCoords( topCoord_BL );
}

BlockDefinition::~BlockDefinition()
{

}

void BlockDefinition::LoadDefinitions()
{
	s_material		= Material::CreateNewFromFile( "Data\\Materials\\block.material" );
	s_spriteSheet	= new SpriteSheet( *s_material->GetTexture(0), 32, 32 );

	s_definitions[ BLOCK_AIR ]			= BlockDefinition( BLOCK_AIR,		false, false, true,	 00,	IntVector2(0, 0),	IntVector2(0, 0),	IntVector2(0, 0) );
	s_definitions[ BLOCK_GRASS ]		= BlockDefinition( BLOCK_GRASS,		true,  true,  false, 00,	IntVector2(3, 3),	IntVector2(4, 3),	IntVector2(1, 0) );
	s_definitions[ BLOCK_DIRT ]			= BlockDefinition( BLOCK_DIRT,		true,  true,  false, 00,	IntVector2(4, 3),	IntVector2(4, 3),	IntVector2(4, 3) );
	s_definitions[ BLOCK_STONE ]		= BlockDefinition( BLOCK_STONE,		true,  true,  false, 00,	IntVector2(0, 4),	IntVector2(0, 4),	IntVector2(0, 4) );
	s_definitions[ BLOCK_GLOWSTONE ]	= BlockDefinition( BLOCK_GLOWSTONE,	true,  false, false, 14,	IntVector2(3, 13),	IntVector2(3, 13),	IntVector2(3, 13) );
	s_definitions[ BLOCK_TEST_WHITE ]	= BlockDefinition( BLOCK_TEST_WHITE,true,  true,  false, 00,	IntVector2(31, 31),	IntVector2(31, 31),	IntVector2(31, 31) );
	s_definitions[ BLOCK_INVALID ]		= BlockDefinition( BLOCK_INVALID,	true,  true,  false, 00,	IntVector2(2, 2),	IntVector2(2, 2),	IntVector2(2, 2) );
}

void BlockDefinition::DestroyDefinitions()
{
	delete s_material;
	s_material = nullptr;

	delete s_spriteSheet;
	s_spriteSheet = nullptr;
}
