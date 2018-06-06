#pragma once
#include "FromFile.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/MathUtil.hpp"

FromFile::FromFile( const XMLElement& genStepXmlElement )
	: MapGenStep( genStepXmlElement )
{
	m_imageName		= ParseXmlAttribute( genStepXmlElement, "imageName", m_imageName );
	m_sourceImage	= new Image( std::string( "Data/Images/" + m_imageName ) );
}

FromFile::~FromFile()
{
	delete m_sourceImage;
	m_sourceImage = nullptr;
}

void FromFile::Run( Map& map )
{
	// Check chanceToRun
	if( CheckRandomChance( m_chanceToRun ) == false )
		return;

	// Some common calculations for this MapGenStep
	IntVector2	sourceDimension		= m_sourceImage->GetDimensions();
	IntVector2	mapDimension		= map.m_dimension;
	int			x_axisExtraTiles	= mapDimension.x - sourceDimension.x;
	int			y_axisExtraTiles	= mapDimension.y - sourceDimension.y;

	// For each iterations
	for( int i = 0; i < m_iterations; i++ )
	{
		// If image is larger than the map
		if( x_axisExtraTiles <= 0 || y_axisExtraTiles <= 0 )
		{
			for( int yMap = mapDimension.y - 1; yMap >= 0; yMap-- )
			{
				for( int xMap = 0; xMap < mapDimension.x; xMap++ )
				{
					// From top-left tile to bottom-right tile of the Map
					int xSource = xMap;
					int ySource = (mapDimension.y - 1) - yMap;

					// If one dimension of sourceImage is smaller, don't skip this iteration
					if( xSource >= sourceDimension.x )
						continue;
					if( ySource >= sourceDimension.y )
						continue;

					// Get texel & change tile
					Rgba			sourceTexel	= m_sourceImage->GetTexel( xSource, ySource );
					TileDefinition*	newTileDef	= TileDefinition::GetTileDefinitionForRGB( sourceTexel );

					if( newTileDef != nullptr )
					{
						// Account for possibility of not changing the Tile
						float possibilityFraction = RangeMapFloat( sourceTexel.a, 0.f, 255.f, 0.f, 1.f );
						if( CheckRandomChance( possibilityFraction ) == false )
							continue;

						// Change the tileDef
						int tileIndex = GetIndexFromColumnRowNumberForMatrixOfWidth( xMap, yMap, mapDimension.x );
						map.m_tiles[ tileIndex ].ChangeTileDef( *newTileDef );
					}
				}
			}
		}
		// Else image is smaller than the map
		else
		{
			// Choose random start coordinate, wisely
			int x_startCoordMap = 0 + GetRandomIntInRange( 0, x_axisExtraTiles - 1 );
			int y_startCoordMap = (mapDimension.y - 1 ) - GetRandomIntInRange( 0, y_axisExtraTiles - 1 );

			// Start changing tiles from that point
			for( int ySource = 0; ySource < sourceDimension.y; ySource++ )
			{
				for ( int xSource = 0; xSource < sourceDimension.x; xSource++ )
				{
					int xMap = x_startCoordMap + xSource;
					int yMap = y_startCoordMap - ySource;

					// Get texel & change tile
					Rgba			sourceTexel	= m_sourceImage->GetTexel( xSource, ySource );
					TileDefinition*	newTileDef	= TileDefinition::GetTileDefinitionForRGB( sourceTexel );

					if( newTileDef != nullptr )
					{
						// Account for possibility of not changing the Tile
						float possibilityFraction = RangeMapFloat( sourceTexel.a, 0.f, 255.f, 0.f, 1.f );
						if( CheckRandomChance( possibilityFraction ) == false )
							continue;

						// Change the tileDef
						int tileIndex = GetIndexFromColumnRowNumberForMatrixOfWidth( xMap, yMap, mapDimension.x );
						map.m_tiles[ tileIndex ].ChangeTileDef( *newTileDef );
					}
				}
			}
		}
	}
}
