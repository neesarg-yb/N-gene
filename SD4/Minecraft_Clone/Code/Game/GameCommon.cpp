#pragma once
#include "GameCommon.hpp"
#include "Game/theApp.hpp"
#include "Game/theGame.hpp"

Blackboard*				g_gameConfigBlackboard	= nullptr;
Clock*					g_gameClock				= nullptr;
theApp*					g_theApp				= nullptr;
theGame*				g_theGame				= nullptr;
Renderer*				g_theRenderer			= nullptr;
Material*				g_defaultMaterial		= nullptr;
InputSystem*			g_theInput				= nullptr;
RemoteCommandService*	g_rcs					= nullptr;
/*
AudioSystem* g_theAudio = nullptr;
*/

eBlockType GetBlockTypeFromInteger( int typeInt )
{
	if( typeInt >= NUM_BLOCK_TYPES || typeInt < 0 )
		return BLOCK_INVALID;
	else
		return (eBlockType) typeInt;
}
