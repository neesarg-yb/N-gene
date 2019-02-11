#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"

//----------
// CONSTANTS
uint constexpr BITS_WIDE_X	= 4;
uint constexpr BITS_WIDE_Y	= 4;
uint constexpr BITS_WIDE_Z	= 8;
uint constexpr BITS_MASK_X	= 0b0000'0000'0000'1111;
uint constexpr BITS_MASK_Y	= 0b0000'0000'1111'0000;
uint constexpr BITS_MASK_Z	= 0b1111'1111'0000'0000;

uint constexpr BLOCKS_WIDE_X	= (1 << BITS_WIDE_X);	// Number of blocks in x-dir of a chunk
uint constexpr BLOCKS_WIDE_Y	= (1 << BITS_WIDE_Y);	// Number of blocks in y-dir of a chunk
uint constexpr BLOCKS_WIDE_Z	= (1 << BITS_WIDE_Z);	// Number of blocks in z-dir of a chunk
uint constexpr NUM_BLOCKS_PER_CHUNK = (BLOCKS_WIDE_X * BLOCKS_WIDE_Y * BLOCKS_WIDE_Z);


//-----------------
// TYPE DEFINITIONS
typedef IntVector2 ChunkCoord;
typedef IntVector3 BlockCoord;


//------
// Enums
enum eBlockType : uchar
{
	BLOCK_AIR = 0,
	BLOCK_GRASS,
	BLOCK_STONE,
	NUM_BLOCK_TYPES
};


//-----------------
// GLOBAL VARIABLES
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
extern Material* g_defaultMaterial;

extern InputSystem* g_theInput;
// extern AudioSystem* g_theAudio;

