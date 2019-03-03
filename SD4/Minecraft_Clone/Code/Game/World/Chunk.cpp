#pragma once
#include "Chunk.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/World/World.hpp"
#include "Game/World/BlockLocator.hpp"
#include "Game/World/BlockDefinition.hpp"
#include "Game/Utility/ChunkFile.hpp"
#include "Game/Utility/ChunkFileHeader.hpp"

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

	// If we there's no .chunk file
	if( LoadFromFile() == false )
	{
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
	}

	// Mark as Dirty
	m_isDirty = true;

	// Isn't modified, no new changes to save
	m_needsSaving = false;
}

Chunk::~Chunk()
{
	if( m_needsSaving )
		SaveToFile();

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

Vector3 Chunk::GetBlockWorldPositionFromIndex( uint blockIndex ) const
{
	BlockCoord blockCoords		= GetBlockCoordFromIndex( blockIndex );
	Vector3 chunkWorldPosition	= Vector3( (float)m_coord.x * (float)BLOCKS_WIDE_X, (float)m_coord.y * (float)BLOCKS_WIDE_Y, 0.f );
	Vector3 blockWorldPosition	= Vector3( chunkWorldPosition.x + (float)blockCoords.x, chunkWorldPosition.y + (float)blockCoords.y, chunkWorldPosition.z + (float)blockCoords.z );

	return blockWorldPosition;
}

void Chunk::SaveToFile() const
{
	std::string const fileName = Stringf( "Chunk_%d,%d.chunk", m_coord.x, m_coord.y );
	std::string const filePath = "Saves\\" + fileName;

	ChunkFileHeader fileHeader;
	fileHeader.SetString4CC( "SMCD" );
	fileHeader.m_version = 1;
	fileHeader.m_chunkBitsX = BITS_WIDE_X;
	fileHeader.m_chunkBitsY = BITS_WIDE_Y;
	fileHeader.m_chunkBitsZ = BITS_WIDE_Z;
	fileHeader.m_format		= 'R';

	ChunkFile chunkFile( fileHeader );
	for( int b = 0; b < NUM_BLOCKS_PER_CHUNK; b++ )
		chunkFile.AddNewBlock( m_blocks[b] );

	chunkFile.SaveToFile( filePath );
}

bool Chunk::LoadFromFile()
{
	std::string const fileName = Stringf( "Chunk_%d,%d.chunk", m_coord.x, m_coord.y );
	std::string const filePath = "Saves\\" + fileName;

	// Try to open the chunk file
	ChunkFile chunkFile;
	bool fileOpened = chunkFile.Open( filePath );

	if( fileOpened == false )
		return false;

	// File is opened, create the chunk from file
	ChunkFileHeader chunkFileHeader;
	bool headerReadSuccess = chunkFile.ReadHeader( chunkFileHeader );

	GUARANTEE_RECOVERABLE( headerReadSuccess, "Could not read the chunk's header!" );
	if( headerReadSuccess == false )
		return false;

	// To set block type from file
	int lastBlockSetAtIndex = -1;

	// Start reading all the blocks
	int	 numBlocksPendingToRead		= chunkFileHeader.GetBlocksCount();
	bool lastBlockReadSuccessfully	= true;
	while( lastBlockReadSuccessfully && (numBlocksPendingToRead > 0) )
	{
		int			numBlocks = 0;
		eBlockType	blockType = BLOCK_AIR;

		// Read from file
		lastBlockReadSuccessfully = chunkFile.GetNextBlocks( numBlocks, blockType );

		// Set these blocks of chunk
		for( int setBlockCount = 0; setBlockCount < numBlocks; setBlockCount++ )
		{
			m_blocks[ ++lastBlockSetAtIndex ].SetType( blockType );
		}

		numBlocksPendingToRead -= numBlocks;
	}

	// If we still have blocks to read, but last attempt was a failure
	if( (numBlocksPendingToRead != 0) && (lastBlockReadSuccessfully == false) )
	{
		GUARANTEE_RECOVERABLE( false, "Block read was a failure from .chunk file!" );
		return false;
	}

	return true;
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
	AABB2	const	uvSide			= blockDef.m_uvSide;
	AABB2	const	uvBottom		= blockDef.m_uvBottom;
	AABB2	const	uvTop			= blockDef.m_uvTop;

	if( blockDef.m_isNeverVisible )
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

	Block const &west = locWest.GetBlock();
	if( west.IsFullyOpaque() == false )
	{
		int indoorLight		= west.GetIndoorLightLevel();
		float lightFraction	= RangeMapFloat( indoorLight / 14.f, 0.f, 1.f, 0.1f, 1.f );
		
		Rgba color;
		color.SetAsFloats( lightFraction, lightFraction, lightFraction );

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
	
	Block const &east = locEast.GetBlock(); 
	if( east.IsFullyOpaque() == false )
	{
		int indoorLight		= east.GetIndoorLightLevel();
		float lightFraction	= RangeMapFloat( indoorLight / 14.f, 0.f, 1.f, 0.1f, 1.f );

		Rgba color;
		color.SetAsFloats( lightFraction, lightFraction, lightFraction );

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
	
	Block const &north = locNorth.GetBlock();
	if( north.IsFullyOpaque() == false )
	{
		int indoorLight		= north.GetIndoorLightLevel();
		float lightFraction	= RangeMapFloat( indoorLight / 14.f, 0.f, 1.f, 0.1f, 1.f );

		Rgba color;
		color.SetAsFloats( lightFraction, lightFraction, lightFraction );

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
	
	Block const &south = locSouth.GetBlock();
	if( south.IsFullyOpaque() == false )
	{
		int indoorLight		= south.GetIndoorLightLevel();
		float lightFraction	= RangeMapFloat( indoorLight / 14.f, 0.f, 1.f, 0.1f, 1.f );

		Rgba color;
		color.SetAsFloats( lightFraction, lightFraction, lightFraction );

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

	Block const &up = locUp.GetBlock();
	if( up.IsFullyOpaque() == false )
	{
		int indoorLight		= up.GetIndoorLightLevel();
		float lightFraction	= RangeMapFloat( indoorLight / 14.f, 0.f, 1.f, 0.1f, 1.f );

		Rgba color;
		color.SetAsFloats( lightFraction, lightFraction, lightFraction );

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

	Block const &down = locDown.GetBlock();
	if( down.IsFullyOpaque() == false )
	{
		int indoorLight		= down.GetIndoorLightLevel();
		float lightFraction	= RangeMapFloat( indoorLight / 14.f, 0.f, 1.f, 0.1f, 1.f );

		Rgba color;
		color.SetAsFloats( lightFraction, lightFraction, lightFraction );

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

void Chunk::SetBlockType( int blockIndex, eBlockType newType )
{
	Block &blockToChange = m_blocks[ blockIndex ];

	if( blockToChange.GetType() != newType )
	{
		blockToChange.SetType( newType );
		blockToChange.UpdateBitflagFromDefinition();

		m_isDirty		= true;
		m_needsSaving	= true;
	}
}

void Chunk::SetBlockType( int xBlockCoord, int yBlockCoord, int zBlockCoord, eBlockType type )
{
	int blockIndex = GetIndexFromBlockCoord( xBlockCoord, yBlockCoord, zBlockCoord );
	SetBlockType( blockIndex, type );
}
