#pragma once
#include "Engine/Math/AABB3.hpp"
#include "Game/GameObject.hpp"

class Terrain;

class EnemyBase : public GameObject
{
public:
	 EnemyBase( Vector2 const &basePositionXZ, Terrain &isInTerrain, int const maxEnemiesCount, float const spawnRate, float const towerHeight );
	~EnemyBase();

public:
	Terrain			&m_parentTerrain;
	Vector2 const	 m_xzPosition;
	Vector3 const	 m_towerSize;
	AABB3			 m_worldBounds;
	int		const	 m_maxEnemiesCount;
	float	const	 m_spawnRate					=	1.f;		// Enemies per second

	float			 m_timeSinceLastEnemySpawned	= 0.f;
	int				 m_currentlyAliveSpawnedEnemies = 0;

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );

	inline void IncreaseChildCount	( int increment = 1 ) { m_currentlyAliveSpawnedEnemies += increment; }
	inline void ReduceChildCount	( int reduction = 1 ) { m_currentlyAliveSpawnedEnemies -= reduction; }
};