#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"

constexpr float FLOAT_ERROR_TOLERANCE_MAPFILE = 0.001f;

class  Blackboard;
extern Blackboard* g_gameConfigBlackboard;

class Clock;
extern Clock* g_gameClock;		// Clock managed by theGame

class theApp;
extern theApp* g_theApp;

class theGame;
extern theGame* g_theGame;

class RemoteCommandService;
extern RemoteCommandService* g_rcs;

extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
/*
extern AudioSystem* g_theAudio;
*/
