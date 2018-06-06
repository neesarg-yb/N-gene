#pragma once
#include "FillAndEdge.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/MathUtil.hpp"

FillAndEdge::FillAndEdge( const XMLElement& genStepXmlElement )
	: MapGenStep( genStepXmlElement )
{
	m_fillTileDef	= ParseXmlAttribute( genStepXmlElement, "fillTile", m_fillTileDef );
	m_edgeTileDef	= ParseXmlAttribute( genStepXmlElement, "edgeTile", m_edgeTileDef );
	m_edgeThickness	= ParseXmlAttribute( genStepXmlElement, "edgeThickness", m_edgeThickness );
}

FillAndEdge::~FillAndEdge()
{

}

void FillAndEdge::Run( Map& map )
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

		// For edges
		for( int rowNum = 0; rowNum < mapHeight; rowNum++ )
		{
			for( int columnNum = 0; columnNum < mapWidth; columnNum++ )
			{
				int tileIndex = GetIndexFromColumnRowNumberForMatrixOfWidth( columnNum, rowNum, mapWidth );
				map.m_tiles[ tileIndex ].ChangeTileDef( *m_edgeTileDef );
			}
		}

		// For inner part
		for( int rowNum = m_edgeThickness; rowNum < mapHeight - m_edgeThickness; rowNum++ )
		{
			for( int columnNum = m_edgeThickness; columnNum < mapWidth - m_edgeThickness; columnNum++ )
			{
				int tileIndex = GetIndexFromColumnRowNumberForMatrixOfWidth( columnNum, rowNum, mapWidth );
				map.m_tiles[ tileIndex ].ChangeTileDef( *m_fillTileDef );
			}
		}
	}
}
