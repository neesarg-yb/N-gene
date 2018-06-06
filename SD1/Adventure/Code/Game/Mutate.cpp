#pragma once
#include <vector>
#include "Mutate.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/MathUtil.hpp"

Mutate::Mutate( const XMLElement& genStepXmlElement )
	: MapGenStep( genStepXmlElement )
{
	m_ifTile	= ParseXmlAttribute( genStepXmlElement, "ifTile",			m_ifTile );
	m_toTile	= ParseXmlAttribute( genStepXmlElement, "toTile",			m_toTile );
	m_chance	= ParseXmlAttribute( genStepXmlElement, "chanceToMutate",	m_chance );

	GUARANTEE_OR_DIE( m_ifTile	 != nullptr, std::string("MapGenStep " + m_name + ": Can't find TileDefinition of ifTile!") );
	GUARANTEE_OR_DIE( m_toTile	 != nullptr, std::string("MapGenStep " + m_name + ": Can't find TileDefinition of toTile!") );

	GUARANTEE_RECOVERABLE( m_chance != 0.f , std::string("MapGenStep " + m_name + ": Provided chance of Mutate is ZERO..!") );
}

Mutate::~Mutate()
{

}

void Mutate::Run( Map& map )
{
	// Check chanceToRun
	if( CheckRandomChance( m_chanceToRun ) == false )
		return;

	// For each iterations
	for( int i = 0; i < m_iterations; i++ )
	{
		// Get vector of index of all tiles whose definition is onTileType
		std::vector< int > availableTilesIndex;
		for( int j = 0; j < (map.m_dimension.x * map.m_dimension.y) - 1.f; j++ )
		{
			if( map.m_tiles[j].m_tileDef == m_ifTile )
			{
				availableTilesIndex.push_back( j );
			}
		}

		// Go through all tiles and mutate according to the chance
		for( int tileIndex : availableTilesIndex )
		{
			if( CheckRandomChance( m_chance ) == false )
				continue;

			map.m_tiles[ tileIndex ].ChangeTileDef( *m_toTile );
		}
	}
}
