#pragma once
#include "Engine/Core/Tags.hpp"

class TileExtraInfo
{
public:
	 TileExtraInfo() { };
	 TileExtraInfo( Tags& tags );
	~TileExtraInfo() { };

public:
	Tags	m_tags;

private:

};