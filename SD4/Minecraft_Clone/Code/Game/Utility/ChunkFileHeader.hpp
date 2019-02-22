#pragma once
#include "Game/GameCommon.hpp"

struct ChunkFileHeader
{
public:
	uchar	m_4cc[4]	 = { 'S', 'M', 'C', 'D' };		// Unique 4CC (SimpleMiner Chunk Data)
	uchar	m_version	 = 1;							// .chunk file format version # (1)
	uchar	m_chunkBitsX = BITS_WIDE_X;					// 2.g. 4 if chunk is (1<<4) == 16 wide on X
	uchar	m_chunkBitsY = BITS_WIDE_Y;
	uchar	m_chunkBitsZ = BITS_WIDE_Z;
	uchar	m_reserved1	 = 0;							// zero, until/unless we need to use it later
	uchar	m_reserved2	 = 0;
	uchar	m_reserved3	 = 0;
	uchar	m_format	 = 'R';							// 'R' (or 82, 0x52) for "RLE Compressed"

public:
	int			GetBlocksCount() const;							// How many total blocks in this chunk?
	std::string	GetString4CC() const;
};
