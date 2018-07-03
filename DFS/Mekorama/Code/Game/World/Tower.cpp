#pragma once
#include "Tower.hpp"
#include "Engine/Math/HeatMap3D.hpp"
#include "Engine/Core/StringUtils.hpp"

Tower::Tower( Vector3 position, std::string towerDefinitionName )
	: GameObject( GAMEOBJECT_TYPE_TOWER )
	, m_definition( *TowerDefinition::s_definitions[ towerDefinitionName ] )
{
	// Set tower's transform
	m_transform.SetPosition( position );

	// Create Blocks
	uint	blocksPerSlice	= m_definition.m_xzDimension.x * m_definition.m_xzDimension.y;
	uint	numSlices		= (uint) ( m_definition.m_blocksDefinitionList.size() / blocksPerSlice );
	
	// Set XYZ dimension
	m_dimensionXYZ			= IntVector3( (int)m_definition.m_xzDimension.x, (int)numSlices, (int)m_definition.m_xzDimension.y );

	// for each slices/layers
	uint nextBlkIdx	= 0;
	for( uint sliceIdx = 0U; sliceIdx < numSlices; sliceIdx++ )
	{
		// columns
		for( uint colIdx = 0U; colIdx < (uint)m_definition.m_xzDimension.y; colIdx++ )
		{
			// rows
			for( uint rowIdx = 0U; rowIdx < (uint)m_definition.m_xzDimension.x; rowIdx++ )
			{
				IntVector3	posInTower	= IntVector3( (int)rowIdx, (int)sliceIdx, (int)colIdx );
				Block*		thiBlock	= new Block( posInTower, m_definition.m_blocksDefinitionList[ nextBlkIdx ], *this );
				m_allBlocks.push_back( thiBlock );

				nextBlkIdx++;
			}
		}
	}

	// Generate extra layer of void/air on top
	m_dimensionXYZ.y += 1;
	for( uint colIdx = 0U; colIdx < (uint)m_definition.m_xzDimension.y; colIdx++ )
	{
		for( uint rowIdx = 0U; rowIdx < (uint)m_definition.m_xzDimension.x; rowIdx++ )
		{
			IntVector3	posInTower	= IntVector3( (int)rowIdx, (int)numSlices, (int)colIdx );
			Block*		thiBlock	= new Block( posInTower, "Air", *this );
			m_allBlocks.push_back( thiBlock );
		}
	}
}

Tower::~Tower()
{
	for( uint i = 0; i < m_allBlocks.size(); i++ )
		delete m_allBlocks[i];

	m_allBlocks.clear();
}

void Tower::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Tower::SetFinishBlockAt( IntVector3 const &finishPos )
{
	int idx = GetIndexOfBlockAt( finishPos );

	m_allBlocks[ idx ]->ChangeBlockTypeTo( "Finish" );
}

uint Tower::GetIndexOfBlockAt( IntVector3 const &blockPos )
{
	IntVector2	towerXZDim			= m_definition.m_xzDimension;
	uint		numBlocksInALayer	= towerXZDim.x * towerXZDim.y;

	uint		idx					= ( blockPos.x )							// rowIdx
									+ ( blockPos.z * towerXZDim.x )				// columnIdx * width
									+ ( blockPos.y * numBlocksInALayer );		// hightIdx  * blocksInALayer

	return idx;
}

Vector3 Tower::GetWorldLocationOfBlockAt( IntVector3 const &blockPos )
{
	uint	blockIdx = GetIndexOfBlockAt( blockPos );
	Vector3 worldPos = m_allBlocks[ blockIdx ]->m_transform.GetWorldPosition();
	
	return worldPos;
}

Block* Tower::GetBlockAt( IntVector3 const &blockPos )
{
	uint idx = GetIndexOfBlockAt( blockPos );

	return m_allBlocks[ idx ];
}

Block* Tower::GetBlockOnTopOfMe( Block &baseBlock )
{
	// Get place in tower of baseBlock
	IntVector3 baseBlockPos		= baseBlock.GetMyPositionInTower();
	IntVector3 targetBlockPos	= baseBlockPos + IntVector3::UP;

	// Get the block's pointer
	Block* targetBlock			= GetBlockAt( targetBlockPos );
	return targetBlock;
}

HeatMap3D* Tower::GetNewHeatMapForTargetPosition( IntVector3 targetPos )
{
	// Create a new HeatMap
	float maxHeatValue = ( m_dimensionXYZ.x * m_dimensionXYZ.y * m_dimensionXYZ.z ) + 10.f;
	GUARANTEE_OR_DIE( maxHeatValue > 0.f, "Error: maxHeatValue overflowed!!" );

	HeatMap3D *newHeatMap = new HeatMap3D( m_dimensionXYZ, maxHeatValue );
	newHeatMap->SetHeat( 0.f, targetPos );


	////////////////////////
	//  Pupulate HeatMap  //
	////////////////////////
	bool aHeatValueGotChanged;

	do 
	{
		aHeatValueGotChanged = false;

		for( int y = 0; y < m_dimensionXYZ.y; y++ )
		{
			for( int z = 0; z < m_dimensionXYZ.z; z++ )
			{
				for( int x = 0; x < m_dimensionXYZ.x; x++ )
				{
					// Traverses along x-axis for each z-value, for each y-layer
					IntVector3	 currentBlockAt = IntVector3( x, y, z );
					Block		&currentBlock	= *m_allBlocks[ GetIndexOfBlockAt( currentBlockAt ) ];

					IntVector3 newsSurroundingBlocks[4] = {		currentBlockAt + IntVector3::FRONT,			// North on this layer
																currentBlockAt + IntVector3::RIGHT,			// East
																currentBlockAt + IntVector3::LEFT,			// West
																currentBlockAt + IntVector3::BACK	};		// South
					// For each N/E/W/S Blocks
					for( int i = 0; i < 4; i++ )
					{
						// Skip if surrounding block is out of bounds
						if( PositionIsOutsideTowersBounds( newsSurroundingBlocks[i] ) )
							continue;

						IntVector3 goalPos	= newsSurroundingBlocks[i];
						Block &goalBlock	= *m_allBlocks[ GetIndexOfBlockAt(goalPos) ];

						// Goal is not solid
						// Goal has a solid/stairs beneath it
						bool hasSolidBlockBeneathGoal	= HasSolidBlockBeneath( goalPos );
						bool hasStairsBeneathGoal		= HasStairsBlockBeneath( goalPos );
						if( goalBlock.m_definition->m_isSolid == false && ( hasSolidBlockBeneathGoal || hasStairsBeneathGoal ) )
						{
							// Change its heat value
							float heatAtGoal		= newHeatMap->GetHeat( goalPos );
							float costToMoveToGoal	= newHeatMap->GetHeat( currentBlockAt ) + 1.f;

							if( costToMoveToGoal < heatAtGoal )
							{
								newHeatMap->SetHeat( costToMoveToGoal, goalPos );
								aHeatValueGotChanged = true;
							}
						}
					}


					// For each Up
					IntVector3 upBlockPos	= currentBlockAt + IntVector3::UP;
					// Ignore if not out of bounds
					if( PositionIsOutsideTowersBounds( upBlockPos ) == false )
					{
						Block &upBlock			= *m_allBlocks[ GetIndexOfBlockAt( upBlockPos ) ];
						// Goal is not solid
						// Current is stairs
						if( upBlock.m_definition->m_isSolid == false && currentBlock.m_definition->m_typeName == "Stairs" )
						{
							// Change its heat value
							float heatAtGoal		= newHeatMap->GetHeat( upBlockPos );
							float costToMoveAtGoal	= newHeatMap->GetHeat( currentBlockAt ) + 1.f;

							if( costToMoveAtGoal < heatAtGoal )
							{
								newHeatMap->SetHeat( costToMoveAtGoal, upBlockPos );
								aHeatValueGotChanged = true;
							}
						}
					}
					
					// For each Down
					IntVector3 botBlockPos	= currentBlockAt + IntVector3::BOTTOM;
					// Ignore if not out of bounds
					if( PositionIsOutsideTowersBounds( botBlockPos ) == false )
					{
						Block &botBlock			= *m_allBlocks[ GetIndexOfBlockAt( botBlockPos ) ];
						// Goal is stairs
						// Current is not solid
						if( botBlock.m_definition->m_typeName == "Stairs" && currentBlock.m_definition->m_isSolid == false )
						{
							// Change its heat value
							float heatAtGoal		= newHeatMap->GetHeat( botBlockPos );
							float costToMoveAtGoal	= newHeatMap->GetHeat( currentBlockAt ) + 1.f;

							if( costToMoveAtGoal < heatAtGoal )
							{
								newHeatMap->SetHeat( costToMoveAtGoal, botBlockPos );
								aHeatValueGotChanged = true;
							}
						}
					}
				}
			}
		}
	} while ( aHeatValueGotChanged == true );

	return newHeatMap;
}

bool Tower::HasSolidBlockBeneath( IntVector3 const &myPosition )
{
	IntVector3 posOfBlockUnderneeth = myPosition + IntVector3::BOTTOM;

	// If that block is out of bounds
	if( PositionIsOutsideTowersBounds( posOfBlockUnderneeth ) )
		return false;

	// Else, return actual value
	uint idx = GetIndexOfBlockAt( posOfBlockUnderneeth );
	return m_allBlocks[ idx ]->m_definition->m_isSolid;
}

bool Tower::HasStairsBlockBeneath( IntVector3 const &myPosition )
{
	IntVector3 posOfBlockUnderneeth = myPosition + IntVector3::BOTTOM;

	// If that block is out of bounds
	if( PositionIsOutsideTowersBounds( posOfBlockUnderneeth ) )
		return false;

	// Else, return actual value
	uint idx = GetIndexOfBlockAt( posOfBlockUnderneeth );
	return ( m_allBlocks[ idx ]->m_definition->m_typeName == "Stairs" );
}

bool Tower::PositionIsOutsideTowersBounds( IntVector3 const &myPosition )
{
	// if block is out of bounds
	if( myPosition.x >= m_dimensionXYZ.x	|| myPosition.y >= m_dimensionXYZ.y	|| myPosition.z >= m_dimensionXYZ.z ||
		myPosition.x < 0					|| myPosition.y < 0					|| myPosition.z < 0 )
	{
		return true;
	}
	else
		return false;
}