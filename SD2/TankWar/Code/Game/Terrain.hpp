#pragma once
#include "Game/GameObject.hpp"

class Terrain : public GameObject
{
public:
	 Terrain( Vector3 spawnPosition, Vector2 size, float maxHeight, float xzScale );
	~Terrain();

public:
	float	m_maxHeight			= 10.f;
	float	m_xzScale			= 1.f;
	AABB2	m_terrainBoundsXZ	= AABB2( -100.f, -100.f, 100.f, 100.f );

public:
	void	Update( float deltaSeconds );
	Vector3 SinWavePlane( float u, float v );
};