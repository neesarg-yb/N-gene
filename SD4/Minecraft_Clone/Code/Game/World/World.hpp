#pragma once
#include <map>
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/World/Chunk.hpp"
#include "Game/Cameras/MCamera.hpp"

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
	int			 m_activationRadius		= 4;	// chunks
	int			 m_deactivationRadius	= 5;	// chunks
	
	// Relative offsets from origin
	std::vector< ChunkCoord > m_activationPriorityCheatSheet;	// Sorted: smallest to largest distance from origin

public:
	void	Update();
	void	Render() const;

private:
	void	ProcessInput( float deltaSeconds );
	void	ActivateChunkNearestToPosition( Vector3 const &playerWorldPos );
	void	DeactivateChunkForPosition( Vector3 const &playerWorldPos );
	void	PopulateChunkActivationCheatsheet( int deactivationRadius );

public:
	static void			RenderBasis( Vector3 const &position, float length, Renderer &activeRenderer );
	static bool			CheetsheetCompare( ChunkCoord const &a, ChunkCoord const &b );
	static ChunkCoord	ChunkCoordFromWorldPosition( Vector3 const &position );
};
