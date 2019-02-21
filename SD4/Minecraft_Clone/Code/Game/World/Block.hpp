#pragma once
#include "Game/GameCommon.hpp"
#include "Game/World/BlockDefinition.hpp"

class Block 
{
public:
	 Block() {}
	~Block() {}

private:
	uchar m_type = BLOCK_AIR;

public:
	void		SetType( eBlockType type );
	eBlockType	GetType() const;
	bool		IsOpaque() const;

public:
	static Block INVALID;
};

inline bool Block::IsOpaque() const
{
	return BlockDefinition::GetDefinitionForType( (eBlockType)m_type ).m_isOpaque;
}
