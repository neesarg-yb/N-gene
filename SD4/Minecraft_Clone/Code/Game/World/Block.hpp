#pragma once
#include "Game/GameCommon.hpp"

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
};
