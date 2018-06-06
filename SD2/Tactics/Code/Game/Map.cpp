#pragma once
#include "Map.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/MathUtil.hpp"

Map::Map( std::string depth_image, int map_height, BlockDefinition* default_block_definition )
{
	Image image_src = Image( std::string("Data\\Images\\") + depth_image );
	IntVector2 xzDimension = image_src.GetDimensions();
	m_dimensions = IntVector3( xzDimension.x, map_height, xzDimension.y );

	// I traverse through xz-plane, up to map's height which is y-axis ( first )
	for( int z = 0; z < m_dimensions.z; z++ )
	{
		for( int x = 0; x < m_dimensions.x; x++ )
		{	// For every block on xz-plane 
			for( int y = 0; y < m_dimensions.y; y++ )
			{	// For all blocks up to the map's max-height
				Rgba texel	= image_src.GetTexel( x, z );
				int  height	= (int) RangeMapFloat( (float) texel.r, 0.f, 255.f, 0.f, (float) m_dimensions.y );

				Vector3 blockPosition	= Vector3( (float) x, (float) y, (float) z );
				BlockDefinition* def	= nullptr;

				if( y < height )
					def	= default_block_definition;		// Create default_blocks
				else
					def = nullptr;						// Create empty_blocks

				Block* blockToAdd = new Block( blockPosition, def );
				m_map_blocks.push_back( blockToAdd );
			}
		}
	}
}

Map::~Map()
{
	for( int i = 0; i < m_map_blocks.size();  )
	{
		delete m_map_blocks[i];
		m_map_blocks.erase( m_map_blocks.begin() + i );
	}
}

void Map::Update( float deltaSeconds )
{
	for( int i = 0; i < m_map_blocks.size(); i++ )
		m_map_blocks[i]->Update( deltaSeconds );
}

void Map::Render() const
{
	for( int i = 0; i < m_map_blocks.size(); i++ )
		m_map_blocks[i]->Render();
}

Vector3 Map::GiveWorldPositionToRenderOnTopOfTheBlockAt( int x, int z ) const 
{
	// Get first block of which m_definition is nullptr at that (x, , z) coordinate
	x = ClampInt( x, 0, m_dimensions.x - 1 );
	z = ClampInt( z, 0, m_dimensions.z - 1 );

	for( int i=0; i<m_dimensions.y; i++ )
	{
		int blockIndex = IndexOfBlockAt( x, i, z );

		if( m_map_blocks[ blockIndex ]->m_definition == nullptr )
			return m_map_blocks[ blockIndex ]->m_position;
	}

	int indexOfTopMostBlock = IndexOfBlockAt( x, m_dimensions.y-1, z );
	return m_map_blocks[ indexOfTopMostBlock ]->m_position;
}

int Map::IndexOfBlockAt( int x, int y, int z ) const
{
	int numBlocksInOneXYPlane	= (m_dimensions.x * m_dimensions.y);

	int indexOnXYPlane			= (x * m_dimensions.y) + y;
	int indexOnXYZPlane			= indexOnXYPlane + ( z * numBlocksInOneXYPlane );

	return indexOnXYZPlane;
}

HeatMap Map::GenerateHeatMapForPositionAndJumpHeight( int x, int z, int jumpHeight ) const
{
	IntVector2	mapXZDimensions = IntVector2( m_dimensions.x, m_dimensions.z );
	HeatMap		theHeatMap		= HeatMap( mapXZDimensions, (float) mapXZDimensions.x * mapXZDimensions.y );
	// Add current position to queue with initial heatValue
	std::vector< IntVector2 > cellQueue;
	
	IntVector2	operatingOnCell = IntVector2( x, z );
	theHeatMap.SetHeat( 0.f, operatingOnCell );
	cellQueue.push_back( operatingOnCell );

	// For next cell in the queue
	for( unsigned int i = 0; i < cellQueue.size(); )
	{
		float heatOfParentCell = theHeatMap.GetHeat( cellQueue[i] );
		int	  yHeightOfParent  = (int) GiveWorldPositionToRenderOnTopOfTheBlockAt( cellQueue[i].x, cellQueue[i].y ).y;
		
		// For all the adjacent cells,
		for( int direction = 0; direction < NUM_DIRECTIONS; direction++ )
		{
			operatingOnCell = GetNeighbourTileCoordinate( cellQueue[i].x, cellQueue[i].y, static_cast< DirectionEnum >( direction ) );

			if( operatingOnCell == IntVector2::MINUS_ONE_XY )
				continue;

			int	  yHeightOfOperatingCell = (int) GiveWorldPositionToRenderOnTopOfTheBlockAt( operatingOnCell.x, operatingOnCell.y).y;
			float newHeat				 = heatOfParentCell + 1.f;
			// If newHeat is < theirHeat
			if( newHeat < theHeatMap.GetHeat( operatingOnCell ) && abs( yHeightOfOperatingCell - yHeightOfParent ) <= jumpHeight )
			{
				// Add them in the queue
				theHeatMap.SetHeat( newHeat, operatingOnCell );
				cellQueue.push_back( operatingOnCell );
			}
		}

		cellQueue.erase( cellQueue.begin() + i );
	}

	return theHeatMap;
}

IntVector2 Map::GetNeighbourTileCoordinate( int x, int z, DirectionEnum direction ) const
{
	int nextX = -1;
	int nextZ = -1;

	switch ( direction )
	{
	case LEFT_DIR:
		nextX = x - 1;
		nextZ = z;
		break;

	case RIGHT_DIR:
		nextX = x + 1;
		nextZ = z;
		break;

	case TOP_DIR:
		nextX = x;
		nextZ = z + 1;
		break;

	case BOTTOM_DIR:
		nextX = x;
		nextZ = z - 1;
		break;

	default:
		nextX = -1;
		nextZ = -1;
		break;
	}

	if( nextX < m_dimensions.x  &&  nextX >= 0  &&  nextZ < m_dimensions.z  &&  nextZ >= 0 )
		return IntVector2( nextX, nextZ );
	else
		return IntVector2::MINUS_ONE_XY;
}