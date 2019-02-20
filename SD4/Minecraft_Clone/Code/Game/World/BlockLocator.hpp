#pragma once
#include "Game/World/Chunk.hpp"

class BlockLocator
{
public:
	 BlockLocator( Chunk *parentChunk, int blockIndex );
	~BlockLocator();

private:
	Chunk	*m_chunk		= nullptr;
	int		 m_blockIndex	= 0;

public:
	Block&		 GetBlock();

	BlockLocator GetUpBlockLocator() const;
	BlockLocator GetDownBlockLocator() const;
	BlockLocator GetWestBlockLocator() const;
	BlockLocator GetEastBlockLocator() const;
	BlockLocator GetSouthBlockLocator() const;
	BlockLocator GetNorthBlockLocator() const;

public:
	static BlockLocator INVALID;
};
