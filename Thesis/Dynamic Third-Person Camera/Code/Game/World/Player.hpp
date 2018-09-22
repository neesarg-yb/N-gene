#pragma once
#include "Game/World/GameObject.hpp"

class Terrain;

class Player : public GameObject
{
public:
	 Player( Vector3 worldPosition, Terrain const &parentTerrain );
	~Player();

public:
	float	const  m_maxSpeed	= 20.f;			// units per seconds
	Terrain const *m_terrain	= nullptr;		// The terrain this player is on

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );

	void AddVelocity( Vector3 velocity );				// Move towards the given direction with my speed
	void AddVelocity( float x, float y, float z );		// Move towards the given direction with my speed
};