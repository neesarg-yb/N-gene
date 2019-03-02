#pragma once
#include "Game/GameCommon.hpp"
#include "Game/World/BlockDefinition.hpp"

class Block 
{
public:
	 Block();
	~Block();

private:
	uchar m_type		= BLOCK_AIR;
	uchar m_lighting	= 0x00;
	uchar m_bitflag		= 0x00;

public:
	eBlockType	GetType() const;
	void		SetType( eBlockType type );
	void		UpdateBitflagFromDefinition();

	bool		IsSky() const;
	void		SetIsSky();
	void		ClearIsSky();

	bool		IsLightDirty() const;
	void		SetIsLightDirty();
	void		ClearIsLightDirty();

	bool		IsFullyOpaque() const;
	void		SetIsFullyOpaque();
	void		ClearIsFullyOpaque();

	bool		IsNeverVisible() const;
	void		SetIsNeverVisible();
	void		ClearIsNeverVisible();

	bool		IsSolid() const;
	void		SetIsSolid();
	void		ClearIsSolid();

public:
	static Block INVALID;
};

inline bool Block::IsSky() const
{
	return BLOCK_BIT_IS_SKY == (m_bitflag & BLOCK_BIT_IS_SKY);
}

inline void Block::SetIsSky()
{
	m_bitflag |= BLOCK_BIT_IS_SKY;
}

inline void Block::ClearIsSky()
{
	m_bitflag &= (~BLOCK_BIT_IS_SKY);
}

inline bool Block::IsLightDirty() const
{
	return BLOCK_BIT_IS_LIGHT_DIRTY == (m_bitflag & BLOCK_BIT_IS_LIGHT_DIRTY);
}

inline void Block::SetIsLightDirty()
{
	m_bitflag |= BLOCK_BIT_IS_LIGHT_DIRTY;
}

inline void Block::ClearIsLightDirty()
{
	m_bitflag &= (~BLOCK_BIT_IS_LIGHT_DIRTY);
}

inline bool Block::IsFullyOpaque() const
{
	return BLOCK_BIT_IS_FULLY_OPAQUE == (m_bitflag & BLOCK_BIT_IS_FULLY_OPAQUE);
}

inline void Block::SetIsFullyOpaque()
{
	m_bitflag |= BLOCK_BIT_IS_FULLY_OPAQUE;
}

inline void Block::ClearIsFullyOpaque()
{
	m_bitflag &= (~BLOCK_BIT_IS_FULLY_OPAQUE);
}

inline bool Block::IsNeverVisible() const
{
	return BLOCK_BIT_IS_NEVER_VISIBLE == (m_bitflag & BLOCK_BIT_IS_NEVER_VISIBLE);
}

inline void Block::SetIsNeverVisible()
{
	m_bitflag |= BLOCK_BIT_IS_NEVER_VISIBLE;
}

inline void Block::ClearIsNeverVisible()
{
	m_bitflag &= (~BLOCK_BIT_IS_NEVER_VISIBLE);
}

inline bool Block::IsSolid() const
{
	return BLOCK_BIT_IS_SOLID == (m_bitflag & BLOCK_BIT_IS_SOLID);
}

inline void Block::SetIsSolid()
{
	m_bitflag |= BLOCK_BIT_IS_SOLID;
}

inline void Block::ClearIsSolid()
{
	m_bitflag &= (~BLOCK_BIT_IS_SOLID);
}
