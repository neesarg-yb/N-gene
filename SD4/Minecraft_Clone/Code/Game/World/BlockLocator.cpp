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

bool BlockLocator::operator==( BlockLocator const &b ) const
{
	BlockLocator const &a = *this;

	return (a.m_chunk == b.m_chunk) && (a.m_blockIndex == b.m_blockIndex);
}

Vector3 BlockLocator::GetBlockWorldPosition() const
{
	if( m_chunk == nullptr )
		return Vector3::ZERO;
	else
		return m_chunk->GetBlockWorldPositionFromIndex( m_blockIndex );
}

AABB3 BlockLocator::GetBlockWorldBounds() const
{
	if( m_chunk == nullptr )
		return AABB3();
	else
		return m_chunk->GetBlockWorldBounds( m_blockIndex, 1.f );
}

void BlockLocator::ChangeTypeTo( eBlockType newType )
{
	if( m_chunk == nullptr )
		return;

	m_chunk->SetBlockType( m_blockIndex, newType );
}

void BlockLocator::SetNeighborBlockChunksDirty()
{
	BlockLocator north = GetNorthBlockLocator();
	BlockLocator east = GetEastBlockLocator();
	BlockLocator south = GetSouthBlockLocator();
	BlockLocator west = GetWestBlockLocator();

	if( north.IsValid() )
		north.GetChunk()->SetDirty();
	if( east.IsValid() )
		east.GetChunk()->SetDirty();
	if( south.IsValid() )
		south.GetChunk()->SetDirty();
	if( west.IsValid() )
		west.GetChunk()->SetDirty();
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
