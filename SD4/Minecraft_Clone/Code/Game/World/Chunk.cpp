#pragma once
#include "Chunk.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Game/World/World.hpp"
#include "Game/World/BlockLocator.hpp"
#include "Game/World/BlockDefinition.hpp"

Chunk::Chunk( ChunkCoord position )
	: m_coord( position )
{
	// World Bounds
	m_worldBounds.mins.x = (float) m_coord.x * BLOCKS_WIDE_X;			// Mins
	m_worldBounds.mins.y = (float) m_coord.y * BLOCKS_WIDE_Y;
	m_worldBounds.mins.z = 0.f;
	m_worldBounds.maxs.x = m_worldBounds.mins.x + (float)BLOCKS_WIDE_X;	// Maxs
	m_worldBounds.maxs.y = m_worldBounds.mins.y + (float)BLOCKS_WIDE_Y;
	m_worldBounds.maxs.z = m_worldBounds.mins.z + (float)BLOCKS_WIDE_Z;

	// Construct Block
	for( int blockY = 0; blockY < BLOCKS_WIDE_Y; blockY++ )
	{
		for( int blockX = 0; blockX < BLOCKS_WIDE_X; blockX++ )
		{
			int		blockZ0Index		= GetIndexFromBlockCoord( blockX, blockY, 0 );
			AABB3	blockZ0WorldBound	= GetBlockWorldBounds( blockZ0Index, 1.f );
			float	perlinNoise			= Compute2dPerlinNoise(blockZ0WorldBound.mins.x, blockZ0WorldBound.mins.y, 300.f, 10);;
			float	seaLevel			= RangeMapFloat( perlinNoise, -1.f, 1.f, 50.f, 128.f );

			for( int blockZ = 0; blockZ < BLOCKS_WIDE_Z; blockZ++ )
			{
				if( blockZ > (int)seaLevel )
					SetBlockType( blockX, blockY, blockZ, BLOCK_AIR );
				else if( blockZ == (int)seaLevel )
					SetBlockType( blockX, blockY, blockZ, BLOCK_GRASS );
				else if( blockZ >= (int)seaLevel - 2 )
					SetBlockType( blockX, blockY, blockZ, BLOCK_DIRT );
				else if( blockZ < (int)seaLevel - 2 )
					SetBlockType( blockX, blockY, blockZ, BLOCK_STONE );
			}
		}
	}

	// Mark as Dirty
	m_isDirty = true;
}

Chunk::~Chunk()
{
	if( m_cpuMesh != nullptr )
		delete m_cpuMesh;

	if( m_gpuMesh != nullptr )
		delete m_gpuMesh;

	m_cpuMesh = nullptr;
	m_gpuMesh = nullptr;
}

void Chunk::Render( Renderer &theRenderer ) const
{
	// Bind Material
	theRenderer.BindMaterialForShaderIndex( *BlockDefinition::GetMaterial() );

	// Draw Mesh
	theRenderer.DrawMesh( *m_gpuMesh, Matrix44() );

	// Indicating the start location
	World::RenderBasis( m_worldBounds.mins, 1.f, theRenderer );
}

void Chunk::RebuildMesh()
{
	// Only rebuild, if dirty
	if( m_isDirty == false )
		return;

	// Delete older meshes
	if( m_cpuMesh != nullptr )
	{
		delete m_cpuMesh;
		m_cpuMesh = nullptr;
	}
	if( m_gpuMesh != nullptr )
	{
		delete m_gpuMesh;
		m_gpuMesh = nullptr;
	}

	// CPU side mesh
	m_cpuMesh = new MeshBuilder();
	m_cpuMesh->Begin( PRIMITIVE_TRIANGES, true );
	for( int bIdx = 0; bIdx < NUM_BLOCKS_PER_CHUNK; bIdx++ )
		AddVertsForBlock( bIdx, *m_cpuMesh );
	m_cpuMesh->End();

	// GPU side mesh
	m_gpuMesh = m_cpuMesh->ConstructMesh<Vertex_3DPCU>();
	
	// Not dirty, anymore..
	m_isDirty = false;
}

void Chunk::SetNeighborAtCoordinate( Chunk *newNeighbor, ChunkCoord const & neighborCoord )
{
	ChunkCoord coordDiff = neighborCoord - m_coord;
	bool aNeighborGotChanged = false;

	if( coordDiff == EAST_CHUNKCOORD )
	{
		// Set as east neighbor
		Chunk* &eastNeighbor = m_neighbor[ EAST_NEIGHBOR_CHUNK ];
		if( eastNeighbor != newNeighbor )
		{
			eastNeighbor = newNeighbor;
			aNeighborGotChanged = true;
		}
	}
	else if( coordDiff == WEST_CHUNKCOORD )
	{
		// Set as west neighbor
		Chunk* &westNeighbor = m_neighbor[ WEST_NEIGHBOR_CHUNK ];
		if( westNeighbor != newNeighbor )
		{
			westNeighbor = newNeighbor;
			aNeighborGotChanged = true;
		}
	}
	else if( coordDiff == NORTH_CHUNKCOORD )
	{
		// Set as north neighbor
		Chunk* &northNeighbor = m_neighbor[ NORTH_NEIGHBOR_CHUNK ];
		if( northNeighbor != newNeighbor )
		{
			northNeighbor = newNeighbor;
			aNeighborGotChanged = true;
		}
	}
	else if( coordDiff == SOUTH_CHUNKCOORD )
	{
		// Set as south neighbor
		Chunk* &southNeighbor = m_neighbor[ SOUTH_NEIGHBOR_CHUNK ];
		if( southNeighbor != newNeighbor )
		{
			southNeighbor = newNeighbor;
			aNeighborGotChanged = true;
		}
	}

	// Mark dirty if a neighbor got changed
	m_isDirty = aNeighborGotChanged ? true : m_isDirty;
}

bool Chunk::HasAllNeighbors() const
{
	for( int nIdx = 0; nIdx < NUM_NEIGHBOR_CHUNKS; nIdx++ )
	{
		if( m_neighbor[nIdx] == nullptr )
			return false;
	}

	return true;
}

int Chunk::GetIndexFromBlockCoord( int xBlockCoord, int yBlockCoord, int zBlockCoord )
{
	return xBlockCoord + (yBlockCoord * BLOCKS_WIDE_X) + (zBlockCoord * (BLOCKS_WIDE_X * BLOCKS_WIDE_Y));
}

AABB3 Chunk::GetBlockWorldBounds( int blockIndex, float blockSize ) const
{
	IntVector3 blockCoordInt = GetBlockCoordFromIndex( blockIndex );

	Vector3 blockWorldMins;
	blockWorldMins.x = m_worldBounds.mins.x + (float)blockCoordInt.x;
	blockWorldMins.y = m_worldBounds.mins.y + (float)blockCoordInt.y;
	blockWorldMins.z = m_worldBounds.mins.z + (float)blockCoordInt.z;

	Vector3 blockWorldMaxs = blockWorldMins + Vector3(blockSize, blockSize, blockSize);

	return AABB3( blockWorldMins, blockWorldMaxs );
}

IntVector3 Chunk::GetBlockCoordFromIndex( uint blockIndex )
{
	int x = (blockIndex & BITS_MASK_X);
	int y = (blockIndex & BITS_MASK_Y) >> BITS_WIDE_X;
	int z = (blockIndex >> (BITS_WIDE_X + BITS_WIDE_Y));

	return IntVector3(x, y, z);
}

Vector3 Chunk::GetBlockWorldPositionFromIndex( uint blockIndex ) const
{
	BlockCoord blockCoords		= GetBlockCoordFromIndex( blockIndex );
	Vector3 blockWorldPosition	= Vector3( (float)(m_coord.x + blockCoords.x), (float)(m_coord.y + blockCoords.y), (float)(0.f + blockCoords.z) );

	return blockWorldPosition;
}

void Chunk::AddVertsForBlock( int blockIndex, MeshBuilder &meshBuilder )
{
	MeshBuilder &mb = meshBuilder;

	Block			&block			= m_blocks[ blockIndex ];
	Vector3	const	size			= Vector3::ONE_ALL;
	Vector3 const	halfDim			= size * 0.5f;
	AABB3	const	blockBounds		= GetBlockWorldBounds( blockIndex, size.x );
	Vector3 const	center			= blockBounds.GetCenter();

	BlockDefinition const &blockDef	= BlockDefinition::GetDefinitionForType( block.GetType() );
	Rgba	const	color			= RGBA_WHITE_COLOR;
	AABB2	const	uvSide			= blockDef.m_uvSide;
	AABB2	const	uvBottom		= blockDef.m_uvBottom;
	AABB2	const	uvTop			= blockDef.m_uvTop;

	if( blockDef.m_type == BLOCK_AIR )
		return;

	/*
	      7_________ 6			VERTEX[8] ORDER:
		  /|       /|				( 0, 1, 2, 3, 4, 5, 6, 7 )
		 / | top  / |				
	   4/__|_____/5 |			
		|  |_____|__|			   z|   
		| 3/     |  /2				|  / x
		| /  bot | /				| /
		|/_______|/			y ______|/ 
		0         1
	*/
	unsigned int idx;
	Vector3 const vertexPos[8] = {
		Vector3( center.x - halfDim.x,	center.y + halfDim.y,	center.z - halfDim.z ),
		Vector3( center.x - halfDim.x,	center.y - halfDim.y,	center.z - halfDim.z ),
		Vector3( center.x + halfDim.x,	center.y - halfDim.y,	center.z - halfDim.z ),
		Vector3( center.x + halfDim.x,	center.y + halfDim.y,	center.z - halfDim.z ),
		Vector3( center.x - halfDim.x,	center.y + halfDim.y,	center.z + halfDim.z ),
		Vector3( center.x - halfDim.x,	center.y - halfDim.y,	center.z + halfDim.z ),
		Vector3( center.x + halfDim.x,	center.y - halfDim.y,	center.z + halfDim.z ),
		Vector3( center.x + halfDim.x,	center.y + halfDim.y,	center.z + halfDim.z )
	};

	BlockLocator locCurrent	= BlockLocator( this, blockIndex );
	BlockLocator locUp		= locCurrent.GetUpBlockLocator();
	BlockLocator locDown	= locCurrent.GetDownBlockLocator();
	BlockLocator locEast	= locCurrent.GetEastBlockLocator();
	BlockLocator locWest	= locCurrent.GetWestBlockLocator();
	BlockLocator locNorth	= locCurrent.GetNorthBlockLocator();
	BlockLocator locSouth	= locCurrent.GetSouthBlockLocator();

	if( locWest.GetBlock().IsOpaque() == false )
	{
		// Back Face (towards you)
		// 4 5
		// 0 1
		mb.SetColor( color );
		mb.SetUV( uvSide.mins.x, uvSide.mins.y );
		idx = mb.PushVertex( vertexPos[0] );									// 0
		mb.SetColor( color );
		mb.SetUV( uvSide.maxs.x, uvSide.mins.y );
		mb.PushVertex( vertexPos[1] );											// 1
		mb.SetColor( color );
		mb.SetUV( uvSide.maxs.x, uvSide.maxs.y );
		mb.PushVertex( vertexPos[5] );											// 5
		mb.SetColor( color );
		mb.SetUV( uvSide.mins.x, uvSide.maxs.y );
		mb.PushVertex( vertexPos[4] );											// 4

		mb.AddFace( idx + 0, idx + 1, idx + 2 );
		mb.AddFace( idx + 2, idx + 3, idx + 0 );
	}
	
	if( locEast.GetBlock().IsOpaque() == false )
	{
		// Front Face (away from you)
		// 6 7
		// 2 3
		mb.SetColor( color );
		mb.SetUV( uvSide.mins.x, uvSide.mins.y );
		idx = mb.PushVertex( vertexPos[2] );									// 2
		mb.SetColor( color );
		mb.SetUV( uvSide.maxs.x, uvSide.mins.y );
		mb.PushVertex( vertexPos[3] );											// 3
		mb.SetColor( color );
		mb.SetUV( uvSide.maxs.x, uvSide.maxs.y );
		mb.PushVertex( vertexPos[7] );											// 7
		mb.SetColor( color );
		mb.SetUV( uvSide.mins.x, uvSide.maxs.y );
		mb.PushVertex( vertexPos[6] );											// 6

		mb.AddFace( idx + 0, idx + 1, idx + 2 );
		mb.AddFace( idx + 2, idx + 3, idx + 0 );
	}
	
	if( locNorth.GetBlock().IsOpaque() == false )
	{
		// Left Face
		// 7 4
		// 3 0
		mb.SetColor( color );
		mb.SetUV( uvSide.mins.x, uvSide.mins.y );
		idx = mb.PushVertex( vertexPos[3] );									// 3
		mb.SetColor( color );
		mb.SetUV( uvSide.maxs.x, uvSide.mins.y );
		mb.PushVertex( vertexPos[0] );											// 0
		mb.SetColor( color );
		mb.SetUV( uvSide.maxs.x, uvSide.maxs.y );
		mb.PushVertex( vertexPos[4] );											// 4
		mb.SetColor( color );
		mb.SetUV( uvSide.mins.x, uvSide.maxs.y );
		mb.PushVertex( vertexPos[7] );											// 7

		mb.AddFace( idx + 0, idx + 1, idx + 2 );
		mb.AddFace( idx + 2, idx + 3, idx + 0 );
	}
	
	if( locSouth.GetBlock().IsOpaque() == false )
	{
		// Right Face
		// 5 6
		// 1 2
		mb.SetColor( color );
		mb.SetUV( uvSide.mins.x, uvSide.mins.y );
		idx = mb.PushVertex( vertexPos[1] );									// 1
		mb.SetColor( color );
		mb.SetUV( uvSide.maxs.x, uvSide.mins.y );
		mb.PushVertex( vertexPos[2] );											// 2
		mb.SetColor( color );
		mb.SetUV( uvSide.maxs.x, uvSide.maxs.y );
		mb.PushVertex( vertexPos[6] );											// 6
		mb.SetColor( color );
		mb.SetUV( uvSide.mins.x, uvSide.maxs.y );
		mb.PushVertex( vertexPos[5] );											// 5

		mb.AddFace( idx + 0, idx + 1, idx + 2 );
		mb.AddFace( idx + 2, idx + 3, idx + 0 );
	}

	if( locUp.GetBlock().IsOpaque() == false )
	{
		// Top Face
		// 7 6
		// 4 5
		mb.SetColor( color );
		mb.SetUV( uvTop.mins.x, uvTop.mins.y );
		idx = mb.PushVertex( vertexPos[4] );									// 4
		mb.SetColor( color );
		mb.SetUV( uvTop.maxs.x, uvTop.mins.y );
		mb.PushVertex( vertexPos[5] );											// 5
		mb.SetColor( color );
		mb.SetUV( uvTop.maxs.x, uvTop.maxs.y );
		mb.PushVertex( vertexPos[6] );											// 6
		mb.SetColor( color );
		mb.SetUV( uvTop.mins.x, uvTop.maxs.y );
		mb.PushVertex( vertexPos[7] );											// 7

		mb.AddFace( idx + 0, idx + 1, idx + 2 );
		mb.AddFace( idx + 2, idx + 3, idx + 0 );
	}

	if( locDown.GetBlock().IsOpaque() == false )
	{
		// Bottom Face
		// 0 1
		// 3 2
		mb.SetColor( color );
		mb.SetUV( uvBottom.mins.x, uvBottom.mins.y );
		idx = mb.PushVertex( vertexPos[3] );									// 3
		mb.SetColor( color );
		mb.SetUV( uvBottom.maxs.x, uvBottom.mins.y );
		mb.PushVertex( vertexPos[2] );											// 2
		mb.SetColor( color );
		mb.SetUV( uvBottom.maxs.x, uvBottom.maxs.y );
		mb.PushVertex( vertexPos[1] );											// 1
		mb.SetColor( color );
		mb.SetUV( uvBottom.mins.x, uvBottom.maxs.y );
		mb.PushVertex( vertexPos[0] );											// 0

		mb.AddFace( idx + 0, idx + 1, idx + 2 );
		mb.AddFace( idx + 2, idx + 3, idx + 0 );
	}
}

void Chunk::SetBlockType( int xBlockCoord, int yBlockCoord, int zBlockCoord, eBlockType type )
{
	int		 blockIndex		= GetIndexFromBlockCoord( xBlockCoord, yBlockCoord, zBlockCoord );
	Block	&blockToChange	= m_blocks[ blockIndex ];

	blockToChange.SetType( type );
}
