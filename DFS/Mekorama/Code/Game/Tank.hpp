#pragma once
#include "Game/GameObject.hpp"

class Terrain;

class Tank : public GameObject
{
public:
	 Tank( Vector3 const &spawnPosition, Terrain &isInTerrain );
	~Tank();

public:
	float m_radius			= 1.f;
	float m_speed			= 10.f;			// units per seconds
	float m_rotationSpeed	= 35.f;	// degrees per seconds
	
	// Debug Trail
	float m_spawnTrailPointAfter = 0.2f;

	// Terrain
	Terrain &m_parentTerrain;

public:
	void Update( float deltaSeconds );
};