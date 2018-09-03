#pragma once
#include "Tower.hpp"
#include <algorithm>
#include "Engine/Math/HeatMap3D.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Profiler/Profiler.hpp"

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

	// Generate the pipes
	for each (PipeSpawnData const &pipeData in m_definition.m_pipeSpawnDataList)
	{
		Pipe *newPipe = new Pipe( pipeData.positionInTower, pipeData.forwardDirection, pipeData.length, *this );
		m_allPipes.push_back( newPipe );
	}
}

Tower::~Tower()
{
	for( uint i = 0; i < m_allPipes.size(); i++ )
		delete m_allPipes[i];

	for( uint i = 0; i < m_allBlocks.size(); i++ )
		delete m_allBlocks[i];

	m_allBlocks.clear();
}

void Tower::Update( float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	UNUSED( deltaSeconds );
}

void Tower::SetFinishBlockAt( IntVector3 const &finishPos )
{
	int idx = GetIndexOfBlockAt( finishPos );

	m_allBlocks[ idx ]->ChangeBlockTypeTo( "Finish" );
}

uint Tower::GetIndexOfBlockAt( IntVector3 const &blockPos ) const
{
	IntVector2	towerXZDim			= m_definition.m_xzDimension;
	uint		numBlocksInALayer	= towerXZDim.x * towerXZDim.y;

	uint		idx					= ( blockPos.x )							// rowIdx
									+ ( blockPos.z * towerXZDim.x )				// columnIdx * width
									+ ( blockPos.y * numBlocksInALayer );		// hightIdx  * blocksInALayer

	return idx;
}

Vector3 Tower::GetWorldLocationOfBlockAt( IntVector3 const &blockPos ) const
{
	uint	blockIdx = GetIndexOfBlockAt( blockPos );
	Vector3 worldPos = m_allBlocks[ blockIdx ]->m_transform.GetWorldPosition();
	
	return worldPos;
}

Block* Tower::GetBlockAt( IntVector3 const &blockPos )
{
	Block* toReturn = nullptr;

	if( IsPositionOutsideTowersBounds( blockPos ) == false )
	{
		uint idx = GetIndexOfBlockAt( blockPos );
		toReturn = m_allBlocks[ idx ];
	}

	return toReturn;
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

Pipe* Tower::GetAnchorPipeForBlock( Block const &block )
{
	for each (Pipe* pipe in m_allPipes)
	{
		float	pipeLength		= pipe->m_length;
		Vector3 pipeStartPos	= pipe->m_startPosition;
		Vector3 pipeEndPos		= pipeStartPos + ( pipe->m_forwardDirection * pipeLength );
		Vector3 blockPos		= Vector3( block.GetMyPositionInTower() );

		Vector3 middleFromStart	= blockPos - pipeStartPos;
		Vector3 middleFromEnd	= blockPos - pipeEndPos;
		Vector3 crossProduct	= Vector3::CrossProduct( middleFromStart, middleFromEnd );
		float	threePointArea	= crossProduct.GetLength();

		// Block is on the same line of the pipe..
		if( threePointArea == 0 )
		{
			// Check if block is in between start and end point
			float endToMiddleDist	= middleFromEnd.GetLength();
			float startToMiddleDist = middleFromStart.GetLength();
			if( startToMiddleDist <= pipeLength && endToMiddleDist <= pipeLength )
				return pipe;
		}
	}

	return nullptr;
}

void Tower::SwapTwoBlocksAt( IntVector3 const &aPos, IntVector3 const &bPos )
{
	uint	aIdx	= GetIndexOfBlockAt( aPos );
	uint	bIdx	= GetIndexOfBlockAt( bPos );

	Block* aBlock	= m_allBlocks[ aIdx ];
	Block* bBlock	= m_allBlocks[ bIdx ];

	// Change position
	aBlock->SetPositionInTower( bPos );
	bBlock->SetPositionInTower( aPos );

	// Change their index in m_allBlocks
	iter_swap( m_allBlocks.begin() + aIdx, m_allBlocks.begin() + bIdx );
}

Block* Tower::DetachAndReplaceWithNewBlockType( Block const &detachBlock, std::string const &replaceDefinition )
{
	// Create a new block from definition
	IntVector3	 pos		= detachBlock.GetMyPositionInTower();
	Block		*newBlock	= new Block( pos, replaceDefinition, *this );

	// Replace it with detachBlock
	uint idx = GetIndexOfBlockAt( pos );
	m_allBlocks[ idx ] = newBlock;

	return newBlock;
}

Block* Tower::DetachBlockAtAndReplaceWith( IntVector3 const &detachBlockAt, Block *replaceWithBlock )
{
	// Detach
	Block*	detachedBlock	= nullptr;
	uint	idx				= GetIndexOfBlockAt( detachBlockAt );
	detachedBlock			= m_allBlocks[ idx ];

	// Replace
	replaceWithBlock->SetPositionInTower( detachBlockAt );
	m_allBlocks[idx] = replaceWithBlock;

	return detachedBlock;
}

std::vector< IntVector3 > Tower::GetNeighbourBlocksPos( IntVector3 const &myPosition )
{
	std::vector< IntVector3 > neighbourPositions;

	IntVector3 stepNorth	= myPosition + IntVector3::FRONT;
	IntVector3 stepSouth	= myPosition + IntVector3::BACK;
	IntVector3 stepRight	= myPosition + IntVector3::RIGHT;
	IntVector3 stepLeft		= myPosition + IntVector3::LEFT;
	IntVector3 stepTop		= myPosition + IntVector3::UP;
	IntVector3 stepBottom	= myPosition + IntVector3::BOTTOM;

	// NORTH
	Block* neighbourBlock = GetBlockAt( stepNorth );
	if( neighbourBlock != nullptr )
		neighbourPositions.push_back( neighbourBlock->GetMyPositionInTower() );

	// SOUTH
	neighbourBlock = GetBlockAt( stepSouth );
	if( neighbourBlock != nullptr )
		neighbourPositions.push_back( neighbourBlock->GetMyPositionInTower() );

	// RIGHT
	neighbourBlock = GetBlockAt( stepRight );
	if( neighbourBlock != nullptr )
		neighbourPositions.push_back( neighbourBlock->GetMyPositionInTower() );

	// LEFT
	neighbourBlock = GetBlockAt( stepLeft );
	if( neighbourBlock != nullptr )
		neighbourPositions.push_back( neighbourBlock->GetMyPositionInTower() );

	// TOP
	neighbourBlock = GetBlockAt( stepTop );
	if( neighbourBlock != nullptr )
		neighbourPositions.push_back( neighbourBlock->GetMyPositionInTower() );

	// BOTTOM
	neighbourBlock = GetBlockAt( stepBottom );
	if( neighbourBlock != nullptr )
		neighbourPositions.push_back( neighbourBlock->GetMyPositionInTower() );

	return neighbourPositions;
}

std::string Tower::GetSurroundingNonSolidBlockDefinitionInSameLayer( Block const &theBlock )
{
	IntVector3 currentBlockAt = theBlock.GetMyPositionInTower();
	
	IntVector3 newsSurroundingBlocks[4] = {		currentBlockAt + IntVector3::FRONT,			// North on this layer
												currentBlockAt + IntVector3::RIGHT,			// East
												currentBlockAt + IntVector3::LEFT,			// West
												currentBlockAt + IntVector3::BACK	};		// South

	for( int i = 0; i < 4; i++ )
	{
		// Skip if surrounding block is out of bounds
		if( IsPositionOutsideTowersBounds( newsSurroundingBlocks[i] ) )
			continue;

		Block* surroundingBlock = GetBlockAt( newsSurroundingBlocks[i] );
		if( surroundingBlock->m_definition->m_isSolid == false )
			return surroundingBlock->m_definition->m_typeName;
	}

	// If we have no clue, let's assume that it was air
	return "Air";
}

HeatMap3D* Tower::GetNewHeatMapForTargetPosition( IntVector3 targetPos ) const
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
						if( IsPositionOutsideTowersBounds( newsSurroundingBlocks[i] ) )
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
					if( IsPositionOutsideTowersBounds( upBlockPos ) == false )
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
					if( IsPositionOutsideTowersBounds( botBlockPos ) == false )
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

bool Tower::HasSolidBlockBeneath( IntVector3 const &myPosition ) const
{
	IntVector3 posOfBlockUnderneeth = myPosition + IntVector3::BOTTOM;

	// If that block is out of bounds
	if( IsPositionOutsideTowersBounds( posOfBlockUnderneeth ) )
		return false;

	// Else, return actual value
	uint idx = GetIndexOfBlockAt( posOfBlockUnderneeth );
	return m_allBlocks[ idx ]->m_definition->m_isSolid;
}

bool Tower::HasStairsBlockBeneath( IntVector3 const &myPosition ) const
{
	IntVector3 posOfBlockUnderneeth = myPosition + IntVector3::BOTTOM;

	// If that block is out of bounds
	if( IsPositionOutsideTowersBounds( posOfBlockUnderneeth ) )
		return false;

	// Else, return actual value
	uint idx = GetIndexOfBlockAt( posOfBlockUnderneeth );
	return ( m_allBlocks[ idx ]->m_definition->m_typeName == "Stairs" );
}

bool Tower::IsPositionOutsideTowersBounds( IntVector3 const &myPosition ) const
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