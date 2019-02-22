#pragma once
#include "ChunkFileHeader.hpp"

int ChunkFileHeader::GetBlocksCount() const
{
	int blocksOnX = 1 << m_chunkBitsX;
	int blocksOnY = 1 << m_chunkBitsY;
	int blocksOnZ = 1 << m_chunkBitsZ;

	return (blocksOnX * blocksOnY * blocksOnZ);
}

std::string ChunkFileHeader::GetString4CC() const
{
	std::string ccString;

	for( int c = 0; c < 4; c++ )
		ccString += m_4cc[c];

	return ccString;
}
