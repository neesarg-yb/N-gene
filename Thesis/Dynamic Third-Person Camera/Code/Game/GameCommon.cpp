#pragma once
#include "GameCommon.hpp"
#include "Game/theApp.hpp"
#include "Game/theGame.hpp"

Blackboard*				g_gameConfigBlackboard	= nullptr;
Clock*					g_gameClock				= nullptr;
theApp*					g_theApp				= nullptr;
theGame*				g_theGame				= nullptr;
Renderer*				g_theRenderer			= nullptr;
InputSystem*			g_theInput				= nullptr;
DebugCamera*			g_activeDebugCamera		= nullptr;
float					g_cameraControlInverted = false;
/*
AudioSystem* g_theAudio = nullptr;
*/