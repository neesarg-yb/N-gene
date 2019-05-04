#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"

//-------------------
// ENUM
//
enum WorldEntityTypes
{
	ENTITY_PLAYER = 0,
	ENTITY_TERRAIN,
	ENTITY_BUILDING,
	ENTITY_HOUSE,
	NUM_ENTITIES
};

enum eSphericalCoordinate : int
{
	SPHERICAL_RADIUS,
	SPHERICAL_ROTATION,
	SPHERICAL_ALTITUDE,
	NUM_SPHERICAL_COORD
};
char const* GetAsString( eSphericalCoordinate coordEnum );

//--------------------
// GLOBAL CONSTANTS
float constexpr PLAYER_MAX_SPEED = 7.f;
float constexpr LOS_RADIUS_REDUCTION = 0.4f;
extern float g_cameraControlInverted;


//--------------------
// GLOBAL VARIABLES
class  Blackboard;
extern Blackboard* g_gameConfigBlackboard;

class Clock;
extern Clock* g_gameClock;		// Clock managed by theGame

class theApp;
extern theApp* g_theApp;

class theGame;
extern theGame* g_theGame;

extern Renderer* g_theRenderer;
extern InputSystem* g_theInput;

class DebugCamera;
extern DebugCamera* g_activeDebugCamera;
/*
extern AudioSystem* g_theAudio;
*/
