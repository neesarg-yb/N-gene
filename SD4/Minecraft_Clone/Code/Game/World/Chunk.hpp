#pragma once
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Game/GameCommon.hpp"
#include "Game/World/Block.hpp"

class BlockLocator;

class Chunk
{
	friend BlockLocator;

public:
	 Chunk( ChunkCoord position );
	~Chunk();

private:
	// Position in world
	ChunkCoord	 m_coord;
	AABB3		 m_worldBounds;

	// Neighbors..
	Chunk		*m_neighbor[ NUM_NEIGHBOR_CHUNKS ] = { nullptr, nullptr, nullptr, nullptr };

	// Blocks
	Block		 m_blocks[ NUM_BLOCKS_PER_CHUNK ];

	// Mesh
	MeshBuilder	*m_cpuMesh = nullptr;
	Mesh		*m_gpuMesh = nullptr;
	bool		 m_isDirty = true;		// i.e. we need to rebuild the CPU & GPU Mesh

public:
	void		Render( Renderer &theRenderer ) const;
	void		RebuildMesh();
	void		SetNeighborAtCoordinate( Chunk *newNeighbor, ChunkCoord const & neighborCoord );
	bool		HasAllNeighbors() const;

	inline void SetDirty() { m_isDirty = true; }
	inline bool	IsDirty() const { return m_isDirty; }
	inline bool HasMesh() const { return m_gpuMesh != nullptr; }

public:
	Vector3	GetBlockWorldPositionFromIndex	( uint blockIndex ) const;
	AABB3	GetBlockWorldBounds				( int blockIndex, float blockSize ) const;

public:
	static	int			GetIndexFromBlockCoord			( int xBlockCoord, int yBlockCoord, int zBlockCoord );
	static	IntVector3	GetBlockCoordFromIndex			( uint blockIndex );
	static	inline int	GetIndexFromBlockCoord			( BlockCoord pos ) { return GetIndexFromBlockCoord( pos.x, pos.y, pos.z ); }

private:
	void		AddVertsForBlock( int blockIndex, MeshBuilder &meshBuilder );
	void		SetBlockType( int xBlockCoord, int yBlockCoord, int zBlockCoord, eBlockType type );
	inline void	SetBlockType( BlockCoord pos, eBlockType type ) { SetBlockType( pos.x, pos.y, pos.z, type ); }
};
