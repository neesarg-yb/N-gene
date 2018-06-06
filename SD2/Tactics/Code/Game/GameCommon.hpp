#pragma once

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"

class	Ability;
typedef std::vector< Ability* > AbilityList;

class  Blackboard;
extern Blackboard*	g_gameConfigBlackboard;

extern Renderer*	g_theRenderer;
extern InputSystem* g_theInput;

class Battle;
extern Battle*		g_currentBattle;

class Map;
extern Map*			g_currentMap;
/*
extern AudioSystem* g_theAudio;
*/