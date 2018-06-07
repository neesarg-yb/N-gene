#pragma once
#include "Game/GameObject.hpp"

class Terrain : public GameObject
{
public:
	 Terrain( Vector3 spawnPosition, Vector2 size );
	~Terrain();

public:
	float	m_maxHeight			= 5.f;
	AABB2	m_terrainBoundsXZ	= AABB2( -100.f, -100.f, 100.f, 100.f );

public:
	void Update( float deltaSeconds );
};