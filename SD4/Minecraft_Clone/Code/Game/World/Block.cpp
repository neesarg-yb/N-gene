#pragma once
#include "Block.hpp"
#include "Engine/Renderer/Renderer.hpp"

Block Block::INVALID = Block();

void Block::SetType( eBlockType type )
{
	m_type = type;
}

eBlockType Block::GetType() const
{
	return eBlockType(m_type);
}
