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

void ChunkFileHeader::SetString4CC( std::string const &str4CC )
{
	if( str4CC.length() < 4 )
		return;

	m_4cc[0] = str4CC[0];
	m_4cc[1] = str4CC[1];
	m_4cc[2] = str4CC[2];
	m_4cc[3] = str4CC[3];
}
