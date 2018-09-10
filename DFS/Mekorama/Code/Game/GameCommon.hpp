#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"

class  Blackboard;
extern Blackboard* g_gameConfigBlackboard;

class theApp;
extern theApp* g_theApp;

class theGame;
extern theGame* g_theGame;

class RemoteCommandService;
extern RemoteCommandService* g_rcs;

class UDPTest;
extern UDPTest *g_udp;

extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;
/*
extern AudioSystem* g_theAudio;
*/