#pragma once
#include "Game/Potential Engine/GameObject.hpp"

class Terrain;

class Player : public GameObject
{
public:
	 Player( Vector3 worldPosition, Terrain const &parentTerrain );
	~Player();

public:
	float	const	 m_bodyRadius	= 0.5f;
	float	const	 m_mass			= 10.f;
	Terrain const	*m_terrain		= nullptr;		// The terrain this player is on

	bool m_isPlayerOnTerrainSurface = false;

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );

	void ApplyResistantForces();
	void ApplyMovementForces();
	void CheckAndSnapOnTerrainSurface();

	void ApplyForce( float x, float y, float z );
	inline void ApplyForce( Vector3 force ) { ApplyForce( force.x, force.y, force.z ); }
};