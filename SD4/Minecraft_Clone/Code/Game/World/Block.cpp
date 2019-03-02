#pragma once
#include "Block.hpp"
#include "Engine/Renderer/Renderer.hpp"

Block Block::INVALID = Block();

Block::Block()
	: m_type( BLOCK_AIR )
{
	ClearIsSolid();
	ClearIsFullyOpaque();

	SetIsNeverVisible();
}

Block::~Block()
{

}

void Block::SetType( eBlockType type )
{
	m_type = type;
}

void Block::UpdateBitflagFromDefinition()
{
	BlockDefinition const &myDefinition = BlockDefinition::GetDefinitionForType( (eBlockType) m_type );

	// Is Fully Opaque
	if( myDefinition.m_isFullyOpaque )
		SetIsFullyOpaque();
	else
		ClearIsFullyOpaque();

	// Is Never Visible
	if( myDefinition.m_isNeverVisible )
		SetIsNeverVisible();
	else
		ClearIsNeverVisible();

	// Is Solid
	if( myDefinition.m_isSolid )
		SetIsSolid();
	else
		ClearIsSolid();
}

eBlockType Block::GetType() const
{
	return eBlockType(m_type);
}
