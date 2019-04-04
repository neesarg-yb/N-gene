#pragma once
#include <deque>
#include <map>
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/World/Chunk.hpp"
#include "Game/Cameras/MCamera.hpp"
#include "Game/Utility/RaycastResult_MC.hpp"

typedef std::map< ChunkCoord, Chunk* >	ChunkMap;
typedef std::deque< BlockLocator >		BlockLocQue;

class World
{
public:
	 World( Clock *parentClock );
	~World();

private:
	// Clock
	Clock		 m_clock;

	// Rendering Specific
	Vector4		 m_ambientLight		= Vector4( 1.f, 1.f, 1.f, 0.7f );

	// Game Camera
	eCameraMode	 m_cameraMode		= CAMERA_DETATCHED;
	MCamera		*m_camera			= nullptr;
	float		 m_flySpeed			= 8.f;
	float		 m_camRotationSpeed = 0.2f;

	// Entities
	ePhysicsMode m_physicsMode		= PHYSICS_NO_CLIP;

	// World Chunks
	ChunkMap	 m_activeChunks;
	int			 m_activationRadius		= ACTIVATION_RANGE_NUM_CHUNKS;	// chunks
	int			 m_deactivationRadius	= m_activationRadius + 2;		// chunks
	
	// Day Night Cycle
	float		 m_worldTimeInDays				= 0.f;
	float const	 m_worldSecondsPerRealSeconds	= 200.f;

	// Debug Mesh
	MeshBuilder	*m_dirtyLightsMesh = nullptr;

	// Lighting
	BlockLocQue	 m_dirtyLightBlocks;									// Block Locater(s) whose lighting is dirty

	// Uniforms for shader
	Vector3	const m_defaultIndoorLight	= Vector3( 1.0f, 0.9f, 0.8f );
	Vector3	const m_defaultOutdoorLight	= Vector3( 0.8f, 0.9f, 1.0f );
	Rgba	const m_defaultSkyColorNight= Rgba(  20,  20,  40 );
	Rgba	const m_defaultSkyColorNoon	= Rgba( 200, 230, 255 );
	float	const m_fogNearDistance		= 100.f;
	float	const m_fogFarDistance		= 200.f;

	// Relative offsets from origin
	std::vector< ChunkCoord > m_activationPriorityCheatSheet;			// Sorted: smallest to largest distance from origin

	// Raycast
	float const			m_raycastMaxDistance			= 8.f;
	bool				m_raycastIsLocked				= false;
	Vector3				m_lockedRayStartPos				= Vector3::ZERO;
	Vector3				m_lockedRayDirection			= Vector3( 1.f, 0.f, 0.f );
	RaycastResult_MC	m_blockSelectionRaycastResult	= RaycastResult_MC();

public:
	void	Update();
	void	Render() const;

public:
	RaycastResult_MC	Raycast( Vector3 const &start, Vector3 const &forwardDir, float maxDistance ) const;
	BlockLocator const	GetBlockLocatorForWorldPosition( Vector3 const &worldPosition ) const;

public:
	void	MarkLightDirtyAndAddUniqueToQueue( BlockLocator &toBeDirtyBlockLoc );
	void	ClearLightDirtyAndRemoveFromQueue( BlockLocator &toBeClearedBlockLoc );

private:
	void	UpdateWorldTime( float deltaSeconds );
	float	GetDaytimeNormalizedUsingSine() const;													// Noon: +1.0 & Midnight: -1.0

	// Input
	void	ProcessInput( float deltaSeconds );
	void	DebugRenderInputKeyInfo() const;
	void	CyclePhysicsMode();
	void	CycleCameraMode();
	
	// Chunk Management
	void	RebuiltOneChunkIfRequired( Vector3 const &playerWorldPos );
	void	ActivateChunkNearestToPosition( Vector3 const &playerWorldPos );
	void	DeactivateChunkForPosition( Vector3 const &playerWorldPos );
	void	GetNeighborsOfChunkAt( ChunkCoord const &chunkCoord, ChunkMap &neighborChunks_out );

	// Volumetric Lighting
	void	UpdateDirtyLighting();
	void	UpdateDirtyLightDebugMesh();
	void	RecomputeLighting( BlockLocator &blockLocator );
	void	GetMaxIncomingLightFromNeighbors( BlockLocator const &receivingBlock, int &maxIndoorLightReceived_out, int &maxOutdoorLightReceived_out ) const;
	void	MarkBlocksLightingDirtyForDig( BlockLocator &targetBlockLoc );							// Call before changing the target block's type
	void	MarkBlocksLightingDirtyForPlace( BlockLocator &targetBlockLoc, eBlockType newType );	// Call before changing the target block's type
	void	MarkNeighborsDirtyForLighting( BlockLocator &thisBlockLoc );
	void	UndirtyAllBlocksLightInChunk( Chunk* chunk );

	void	RenderDirtyLightMesh() const;

	// Glow Strength
	float	GetGlowStrength() const;

	// Sky Color
	Rgba	GetSkyColorFromDayTimeFraction() const;
	float	GetLightningStrengthFromPerlineNoise() const;
	Vector3	GetSkyColorUniformFromPerlineNoiseForLightning( Rgba skyColor ) const;

	// Pre-computation
	void	PopulateChunkActivationCheatsheet( int deactivationRadius );
	
	// Actions on Block
	void	PerformRaycast();
	void	PlaceOrDigBlock();
	void	RenderBlockSelection( RaycastResult_MC const &raycastResult ) const;

public:
	static bool			CheetsheetCompare( ChunkCoord const &a, ChunkCoord const &b );
	static ChunkCoord	ChunkCoordFromWorldPosition( Vector3 const &position );
};
