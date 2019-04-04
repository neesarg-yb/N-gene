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

std::string GetAsString( ePhysicsMode physicsMode )
{
	switch (physicsMode)
	{
	case PHYSICS_NO_CLIP:
		return "No Clip";
	case PHYSICS_FLY:
		return "Fly";
	case PHYSICS_WALK:
		return "Walk";
	default:
		return "INVALID";
	}
}

std::string GetAsString( eCameraMode cameraMode )
{
	switch (cameraMode)
	{
	case CAMERA_DETATCHED:
		return "Detached";
	case CAMERA_1ST_PERSON:
		return "1st Person";
	case CAMERA_OVER_THE_SHOULDER:
		return "Over the Shoulder";
	case CAMERA_FIXED_ANGLE:
		return "Fixed Angle";
	default:
		return "INVALID";
	}
}

