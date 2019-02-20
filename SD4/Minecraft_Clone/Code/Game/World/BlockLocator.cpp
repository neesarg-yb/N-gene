#pragma once
#include "BlockLocator.hpp"

BlockLocator BlockLocator::INVALID = BlockLocator( nullptr, 0 );

BlockLocator::BlockLocator( Chunk *parentChunk, int blockIndex )
	: m_chunk( parentChunk )
	, m_blockIndex( blockIndex )
{

}

BlockLocator::~BlockLocator()
{

}

Block& BlockLocator::GetBlock()
{
	if( m_chunk == nullptr )
		return Block::INVALID;
	else
		return m_chunk->m_blocks[ m_blockIndex ];
}

BlockLocator BlockLocator::GetUpBlockLocator() const
{
	if( (m_blockIndex & BITS_MASK_Z) != BITS_MASK_Z )
		return BlockLocator( m_chunk, m_blockIndex + (BITS_MASK_X + BITS_MASK_Y + 1) );
	else
		return BlockLocator::INVALID;
}

BlockLocator BlockLocator::GetDownBlockLocator() const
{
	if( (m_blockIndex & BITS_MASK_Z) != 0x00 )
		return BlockLocator( m_chunk, m_blockIndex - (BITS_MASK_X + BITS_MASK_Y + 1) );
	else
		return BlockLocator::INVALID;
}

BlockLocator BlockLocator::GetWestBlockLocator() const
{
	if( (m_blockIndex & BITS_MASK_X) != 0x00 )
		return BlockLocator( m_chunk, m_blockIndex - 0x01 );
	else
		return BlockLocator( m_chunk->m_neighbor[WEST_NEIGHBOR_CHUNK], m_blockIndex | BITS_MASK_X );
}

BlockLocator BlockLocator::GetEastBlockLocator() const
{
	if( (m_blockIndex & BITS_MASK_X) != BITS_MASK_X )
		return BlockLocator( m_chunk, m_blockIndex + 0x01 );
	else
		return BlockLocator( m_chunk->m_neighbor[EAST_NEIGHBOR_CHUNK], m_blockIndex & (~BITS_MASK_X) );
}

BlockLocator BlockLocator::GetSouthBlockLocator() const
{
	if( (m_blockIndex & BITS_MASK_Y) != 0x00 )
		return BlockLocator( m_chunk, m_blockIndex - (BITS_MASK_X + 1) );
	else
		return BlockLocator( m_chunk->m_neighbor[SOUTH_NEIGHBOR_CHUNK], m_blockIndex | BITS_MASK_Y );
}

BlockLocator BlockLocator::GetNorthBlockLocator() const
{
	if( (m_blockIndex & BITS_MASK_Y) != BITS_MASK_Y )
		return BlockLocator( m_chunk, m_blockIndex + (BITS_MASK_X + 1) );
	else
		return BlockLocator( m_chunk->m_neighbor[NORTH_NEIGHBOR_CHUNK], m_blockIndex & (~BITS_MASK_Y) );
}
