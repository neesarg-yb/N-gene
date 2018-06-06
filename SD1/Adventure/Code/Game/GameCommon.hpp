#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/XMLUtilities.hpp"

#define UNUSED(x) (void)(x);

class theGame;

extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
extern SpriteSheet* g_tileSpriteSheet;
extern theGame* g_theGame;
/*
extern AudioSystem* g_theAudio;
*/

enum EntityType
{
	ENTITY_TYPE_PROJECTILE,
	ENTITY_TYPE_PORTAL,
	ENTITY_TYPE_NP_ACTOR,		// Non-player Actor
	ENTITY_TYPE_PLAYER,
	NUM_ENTITY_TYPES
};

enum Faction
{
	UNKNOWN = -1,
	GOOD,
	EVIL,
	NUM_FACTIONS
};

Faction ParseXmlAttribute( const XMLElement& element, const char* attributeName, Faction defaultValue );