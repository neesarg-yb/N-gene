#pragma once

#include <map>
#include <string>
#include "Engine/../ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

class TileDefinition
{
public:

	std::string m_tileName = "Name not assigned";
	Tags		m_startTags;
	
	AABB2		m_baseTileUVTextBounds		= AABB2( Vector2( 0.f, 0.f ), 10.f, 10.f );
	Rgba		m_baseTileSpriteTint		= Rgba( 255, 0, 0, 255 );
	bool		m_overlayEnabled			= false;
	AABB2		m_overlayTileUVTextBounds	= AABB2( Vector2( 0.f, 0.f ), 10.f, 10.f );
	Rgba		m_overlayTileSpriteTint		= Rgba( 255, 255, 255, 128 );


	Rgba		m_texelValue = Rgba( 0, 0, 0, 255 );
	bool		m_allowsSight = false;
	bool		m_allowsWalking = false;
	bool		m_allowsFlying = false;
	bool		m_allowsSwimming = false;

	static std::map< std::string, TileDefinition* >		s_definitions;

	static void				LoadTileDefinitions( const XMLElement& root );
	static TileDefinition*	GetTileDefinitionForRGB( const Rgba& texel );			// Note: It ignores alpha value of the texel!

private:
	TileDefinition();
	TileDefinition( const XMLElement& root );
};

TileDefinition*	ParseXmlAttribute( const XMLElement& element, const char* attributeName, TileDefinition* defaultValue );