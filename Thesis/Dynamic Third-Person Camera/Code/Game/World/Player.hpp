#pragma once
#include "Game/World/GameObject.hpp"

class Terrain;

class Player : public GameObject
{
public:
	 Player( Vector3 worldPosition, Terrain const &parentTerrain );
	~Player();

public:
	float	const	 m_mass		= 1.f;
	Terrain const	*m_terrain	= nullptr;		// The terrain this player is on

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );

	void MotionDragUpdate	( float deltaSeonds );
	void MovementUpdate		( float deltaSeconds );
	void CheckAndSnapOnTerrainSurface();

	void ApplyForce( float x, float y, float z );
	inline void ApplyForce( Vector3 force ) { ApplyForce( force.x, force.y, force.z ); }
};