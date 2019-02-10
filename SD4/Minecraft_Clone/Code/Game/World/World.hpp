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
	float		 m_flySpeed			= 5.f;
	float		 m_camRotationSpeed = 0.2f;

	// Gameplay
	ChunkMap	 m_activeChunks;

public:
	void	Update();
	void	Render() const;
	void	RenderBasis( float length ) const;

public:
	void	ProcessInput( float deltaSeconds );
	Block*	GetChunkAtChunkCoordinates( IntVector2 const &chunckCoord );
};
