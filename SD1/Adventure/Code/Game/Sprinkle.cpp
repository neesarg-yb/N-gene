#pragma once
#include <vector>
#include "Sprinkle.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/MathUtil.hpp"

Sprinkle::Sprinkle( const XMLElement& genStepXmlElement )
	: MapGenStep( genStepXmlElement )
{
	m_sprinkleTile	= ParseXmlAttribute( genStepXmlElement, "withTile", m_sprinkleTile );
	m_fraction		= ParseXmlAttribute( genStepXmlElement, "fraction", m_fraction );
	m_edgeThickness	= ParseXmlAttribute( genStepXmlElement, "edgeThickness", m_edgeThickness );

	m_fraction		= ClampFloat01(m_fraction);
}

Sprinkle::~Sprinkle()
{

}

void Sprinkle::Run( Map& map )
{
	// Check chanceToRun
	if( CheckRandomChance( m_chanceToRun ) == false )
		return;

	// For each iterations
	for( int i = 0; i < m_iterations; i++ )
	{
		// Performs its custom operation
		int mapWidth = map.m_dimension.x;
		int mapHeight = map.m_dimension.y;
		std::vector< Tile* > tilesToWorkOn;

		// For inner part
		for( int rowNum = m_edgeThickness; rowNum < mapHeight - m_edgeThickness; rowNum++ )
		{
			for( int columnNum = m_edgeThickness; columnNum < mapWidth - m_edgeThickness; columnNum++ )
			{
				int tileIndex = GetIndexFromColumnRowNumberForMatrixOfWidth( columnNum, rowNum, mapWidth );
				tilesToWorkOn.push_back( &map.m_tiles[ tileIndex ] );
			}
		}

		// Get total number of tiles to work on
		int totalTilesAsPerFraction = (int)(tilesToWorkOn.size() * m_fraction);

		// Choose one of them randomly, and change it
		for( int tileNum = 0; tileNum < totalTilesAsPerFraction; tileNum++ )
		{
			// Get a random tile & change it
			int randIndex = GetRandomIntInRange( 0, tilesToWorkOn.size() - 1 );
			tilesToWorkOn[ randIndex ]->ChangeTileDef( *m_sprinkleTile );
			tilesToWorkOn.erase( tilesToWorkOn.begin() + randIndex );
		}
	}
}
