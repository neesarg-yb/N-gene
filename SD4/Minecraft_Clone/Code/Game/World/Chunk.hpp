#pragma once
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/GameCommon.hpp"
#include "Game/World/Block.hpp"

class Chunk
{
public:
	 Chunk( ChunkCoord position );
	~Chunk();

private:
	// Position in world
	ChunkCoord	m_coord;
	AABB3		m_worldBounds;

	// Blocks
	Block m_blocks[ NUM_BLOCKS_PER_CHUNK ];

	// Mesh
	MeshBuilder		*m_cpuMesh = nullptr;
	Mesh			*m_gpuMesh = nullptr;

public:
	void		Render( Renderer &theRenderer ) const;
	void		RebuildMesh();

public:
	int			GetIndexFromBlockCoord	( int xBlockCoord, int yBlockCoord, int zBlockCoord ) const;
	IntVector3	GetBlockCoordFromIndex	( uint blockIndex ) const;
	inline int	GetIndexFromBlockCoord	( BlockCoord pos ) const { return GetIndexFromBlockCoord( pos.x, pos.y, pos.z ); }
	AABB3		GetBlockWorldBounds		( int blockIndex, float blockSize ) const;

private:
	void		AddVertsForBlock( int blockIndex, MeshBuilder &meshBuilder );
	void		SetBlockType( int xBlockCoord, int yBlockCoord, int zBlockCoord, eBlockType type );
	inline void	SetBlockType( BlockCoord pos, eBlockType type ) { SetBlockType( pos.x, pos.y, pos.z, type ); }
};
