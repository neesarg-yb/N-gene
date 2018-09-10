#pragma once
#include "GameCommon.hpp"
#include "Game/theApp.hpp"
#include "Game/theGame.hpp"

Blackboard*				g_gameConfigBlackboard	= nullptr;
theApp*					g_theApp				= nullptr;
theGame*				g_theGame				= nullptr;
Renderer*				g_theRenderer			= nullptr;
InputSystem*			g_theInput				= nullptr;
RemoteCommandService*	g_rcs					= nullptr;
UDPTest*				g_udp					= nullptr;
/*
AudioSystem* g_theAudio = nullptr;
*/