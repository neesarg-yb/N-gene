#pragma once
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"


//-----------------
// TYPE DEFINITIONS
typedef IntVector2 ChunkCoord;
typedef IntVector3 BlockCoord;


//----------
// CONSTANTS
uint constexpr BITS_WIDE_X	= 4;
uint constexpr BITS_WIDE_Y	= 4;
uint constexpr BITS_WIDE_Z	= 8;
uint constexpr BITS_MASK_X	= 0b0000'0000'0000'1111;
uint constexpr BITS_MASK_Y	= 0b0000'0000'1111'0000;
uint constexpr BITS_MASK_Z	= 0b1111'1111'0000'0000;

uint constexpr BLOCKS_WIDE_X	= (1 << BITS_WIDE_X);			// Number of blocks in x-dir of a chunk
uint constexpr BLOCKS_WIDE_Y	= (1 << BITS_WIDE_Y);			// Number of blocks in y-dir of a chunk
uint constexpr BLOCKS_WIDE_Z	= (1 << BITS_WIDE_Z);			// Number of blocks in z-dir of a chunk
uint constexpr NUM_BLOCKS_PER_CHUNK = (BLOCKS_WIDE_X * BLOCKS_WIDE_Y * BLOCKS_WIDE_Z);

uchar constexpr BLOCK_LIGHT_MASK_INDOOR		= 0b0000'1111;
uchar constexpr BLOCK_LIGHT_MASK_OUTDOOR	= 0b1111'0000;

uchar constexpr BLOCK_BIT_IS_SKY			= 0b1000'0000;		// I am non-opaque & no opaque blocks directly above me
uchar constexpr BLOCK_BIT_IS_LIGHT_DIRTY	= 0b0100'0000;		// A BlockLocator for me is currently in the dirty light opaque
uchar constexpr BLOCK_BIT_IS_FULLY_OPAQUE	= 0b0010'0000;		// I block light, visibility, and hide my neighbors' faces
uchar constexpr BLOCK_BIT_IS_NEVER_VISIBLE	= 0b0001'0000;		// Skip me during chunk mesh rebuilding, because I'm always invisible like "AIR"
uchar constexpr BLOCK_BIT_IS_SOLID			= 0b0000'1000;		// Physical objects and raycasts collide with me

ChunkCoord const EAST_CHUNKCOORD  = ChunkCoord(  1,  0 );
ChunkCoord const WEST_CHUNKCOORD  = ChunkCoord( -1,  0 );
ChunkCoord const NORTH_CHUNKCOORD = ChunkCoord(  0,  1 );
ChunkCoord const SOUTH_CHUNKCOORD = ChunkCoord(  0, -1 );

int constexpr ACTIVATION_RANGE_NUM_CHUNKS	= 13;

// Debug
bool constexpr DEBUG_STEP_LIGHTING			= false;
bool constexpr DEBUG_RENDER_DIRTY_LIGHTS	= false;


//------
// Enums
enum eBlockType : uchar
{
	BLOCK_AIR = 0,
	BLOCK_GRASS,
	BLOCK_DIRT,
	BLOCK_STONE,
	BLOCK_GLOWSTONE,
	BLOCK_TEST_WHITE,
	BLOCK_INVALID,
	NUM_BLOCK_TYPES
};
eBlockType GetBlockTypeFromInteger( int typeInt );

enum eNeighborChunkDirection
{
	NORTH_NEIGHBOR_CHUNK = 0,
	WEST_NEIGHBOR_CHUNK,
	SOUTH_NEIGHBOR_CHUNK,
	EAST_NEIGHBOR_CHUNK,
	NUM_NEIGHBOR_CHUNKS
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

extern Renderer* g_theRenderer;
extern Material* g_defaultMaterial;

extern InputSystem* g_theInput;
// extern AudioSystem* g_theAudio;
