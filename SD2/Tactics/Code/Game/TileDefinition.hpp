#pragma once
#include <map>
#include "Game/GameCommon.hpp"

class TileDefinition
{
public:
	static void LoadTileDefinations( const XMLElement& root );
	static void DeleteAllTheDefinitions();

public:
	static std::map< std::string, TileDefinition* >	s_tileDefinitions;

public:
	Vector3		m_dimension			= Vector3::ZERO;
	Vector3		m_anchorBoxSize		= Vector3::ZERO;
	Vector3		m_anchorPoint		= Vector3::ZERO;

	Rgba		m_tintColor			= RGBA_WHITE_COLOR;
	float		m_yOffsetForDraw	= 0.f;

private:
	 TileDefinition( const XMLElement& root );
	~TileDefinition();
};