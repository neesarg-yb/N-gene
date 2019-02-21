#pragma once
#include <map>
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/World/Chunk.hpp"
#include "Game/Cameras/MCamera.hpp"
#include "Game/Utility/RaycastResult_MC.hpp"

typedef std::map< ChunkCoord, Chunk* > ChunkMap;

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
	MCamera		*m_camera			= nullptr;
	float		 m_flySpeed			= 8.f;
	float		 m_camRotationSpeed = 0.2f;

	// Gameplay
	ChunkMap	 m_activeChunks;
	int			 m_activationRadius		= 20;	// chunks
	int			 m_deactivationRadius	= 22;	// chunks
	
	// Relative offsets from origin
	std::vector< ChunkCoord > m_activationPriorityCheatSheet;	// Sorted: smallest to largest distance from origin

	// Raycast
	float const			m_raycastMaxDistance			= 10.f;
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

private:
	void	ProcessInput( float deltaSeconds );
	void	RebuiltOneChunkIfRequired( Vector3 const &playerWorldPos );
	void	ActivateChunkNearestToPosition( Vector3 const &playerWorldPos );
	void	DeactivateChunkForPosition( Vector3 const &playerWorldPos );
	void	PopulateChunkActivationCheatsheet( int deactivationRadius );
	void	GetNeighborsOfChunkAt( ChunkCoord const &chunkCoord, ChunkMap &neighborChunks_out );
	
	void	PerformRaycast();
	void	RenderBlockSelection( RaycastResult_MC const &raycastResult ) const;

public:
	static void			RenderBasis( Vector3 const &position, float length, Renderer &activeRenderer );
	static void			RenderRaycast( RaycastResult_MC const &raycastResult, Renderer &activeRenderer );
	static void			RenderLineXRay( Vector3 const &startPos, Rgba const &startColor, Vector3 const &endPos, Rgba const &endColor, Renderer &activeRenderer );
	static bool			CheetsheetCompare( ChunkCoord const &a, ChunkCoord const &b );
	static ChunkCoord	ChunkCoordFromWorldPosition( Vector3 const &position );
};
