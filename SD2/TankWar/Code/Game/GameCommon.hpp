#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class  Blackboard;
extern Blackboard* g_gameConfigBlackboard;

class theApp;
extern theApp* g_theApp;

class theGame;
extern theGame* g_theGame;

extern Renderer*	g_theRenderer;
extern InputSystem* g_theInput;
extern AudioSystem* g_theAudioSystem;
