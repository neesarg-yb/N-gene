#pragma once
#include "GameCommon.hpp"

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
SpriteSheet* g_tileSpriteSheet = nullptr;
theGame* g_theGame = nullptr;
/*
AudioSystem* g_theAudio = nullptr;
*/



Faction ParseXmlAttribute( const XMLElement& element, const char* attributeName, Faction defaultValue )
{
	std::string factionStr = "";
	factionStr = ::ParseXmlAttribute(element, attributeName, factionStr);

	// If no such attribute found
	if( factionStr == "" )
		return defaultValue;

	// If faction's name is in known list
	if( factionStr == "good" )
		return Faction::GOOD;
	if( factionStr == "evil" )
		return Faction::EVIL;

	// If flow reaches here, that means faction's name is not in list
	return Faction::UNKNOWN;
}