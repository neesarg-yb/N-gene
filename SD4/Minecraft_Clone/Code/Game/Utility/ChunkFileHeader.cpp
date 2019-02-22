#pragma once
#include "ChunkFileHeader.hpp"

int ChunkFileHeader::GetBlocksCount() const
{
	int blocksOnX = 1 << m_chunkBitsX;
	int blocksOnY = 1 << m_chunkBitsY;
	int blocksOnZ = 1 << m_chunkBitsZ;

	return (blocksOnX * blocksOnY * blocksOnZ);
}
