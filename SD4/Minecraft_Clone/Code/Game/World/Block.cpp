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

	ClearIsLightDirty();
}

Block::~Block()
{

}

void Block::SetType( eBlockType type )
{
	m_type = type;
	UpdateBitflagFromDefinition();
}

int Block::GetIndoorLightLevelFromDefinition() const
{
	eBlockType type = GetBlockTypeFromInteger( m_type );
	BlockDefinition const &blockDef = BlockDefinition::GetDefinitionForType( type );

	return blockDef.m_indoorLightLevel;
}

void Block::UpdateBitflagFromDefinition()
{
	eBlockType type = GetBlockTypeFromInteger( m_type );
	BlockDefinition const &myDefinition = BlockDefinition::GetDefinitionForType( type );

	// Set the indoor lighting
	m_lighting |= ( BLOCK_LIGHT_MASK_INDOOR & (uchar)m_lighting );

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
	return GetBlockTypeFromInteger(m_type);
}

bool Block::DoesEmitLight() const
{
	BlockDefinition const &def = BlockDefinition::GetDefinitionForType( GetType() );
	return def.m_indoorLightLevel > 0;
}
