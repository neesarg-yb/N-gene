#pragma once
#include "Game/GameObject.hpp"

class Terrain;

class Tank : public GameObject
{
public:
	 Tank( Vector2 const &spawnPosition, Terrain &isInTerrain );
	~Tank();

public:
	// XZ Plane info
	Vector2	m_xzPosition	= Vector2::ZERO;
	Vector2 m_xzForward		= Vector2( 0.f, 1.f );
	Vector2 m_xzRight		= Vector2( 1.f, 0.f );

	float m_height			= 1.f;
	float m_speed			= 10.f;	// units per seconds
	float m_rotationSpeed	= 35.f;	// degrees per seconds
	
	// Debug Trail
	float m_spawnTrailPointAfter = 0.2f;

	// Terrain
	Terrain &m_parentTerrain;

public:
	void Update( float deltaSeconds );

private:
	void HandleInput( float deltaSeconds );
};