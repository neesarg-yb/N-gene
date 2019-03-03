#pragma once
#include <deque>
#include "Game/World/Chunk.hpp"

typedef std::deque< BlockLocator > BlockLocQue;

class BlockLocator
{
public:
	 BlockLocator( Chunk *parentChunk, int blockIndex );
	~BlockLocator();

private:
	Chunk	*m_chunk		= nullptr;
	int		 m_blockIndex	= 0;

public:
	bool operator == ( BlockLocator const &b ) const;

public:
	// Self
	bool		 IsValid() const;
	Block&		 GetBlock();
	Block const& GetBlock() const;
	Vector3		 GetBlockWorldPosition() const;
	void		 ChangeTypeTo( eBlockType newType );

	// Chunk
	Chunk*		 GetChunk();

	// Neighbors
	void		 SetNeighborBlockChunksDirty();
	void		 MarkNeighborsDirtyForLighting( BlockLocQue &activeDirtyBlocksQue );

	BlockLocator GetUpBlockLocator() const;
	BlockLocator GetDownBlockLocator() const;
	BlockLocator GetWestBlockLocator() const;
	BlockLocator GetEastBlockLocator() const;
	BlockLocator GetSouthBlockLocator() const;
	BlockLocator GetNorthBlockLocator() const;

public:
	static BlockLocator INVALID;
};

inline bool BlockLocator::IsValid() const
{
	return (m_chunk != nullptr);
}

inline Chunk* BlockLocator::GetChunk()
{
	return m_chunk;
}

inline Block& BlockLocator::GetBlock()
{
	if( m_chunk == nullptr )
		return Block::INVALID;
	else
		return m_chunk->m_blocks[ m_blockIndex ];
}

inline Block const& BlockLocator::GetBlock() const
{
	if( m_chunk == nullptr )
		return Block::INVALID;
	else
		return m_chunk->m_blocks[ m_blockIndex ];
}
