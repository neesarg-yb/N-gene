#pragma once
#include "Game/GameObject.hpp"

class Terrain;
class EnemyBase;

class Enemy : public GameObject
{
public:
	 Enemy( Vector2 const &spawnPosition, Terrain &isInTerrain, EnemyBase &parentBase );
	~Enemy();

public:
	Vector2		 m_currentPositionXZ	=	Vector2::ZERO;
	Vector2		 m_currentVelocityXZ	=	Vector2( 1.f, 0.f );	// Note: Gets normalize before taking it in account
	Terrain		&m_paerntTerrain;
	EnemyBase	&m_parentBase;

	float		 m_radius				= 1.f;
	float		 m_speed				= 8.f;						// units per seconds

public:
	void	Update( float deltaSeconds );
	void	AddRenderablesToScene( Scene &activeScene );

	// Flocking Behavior
	void	AddToVelocity	( Vector2 const &velToAdd );
	void	SteerTowards	( Vector2 const &targetPos );

private:
	Vector3 Get3DRotation( Vector2 xzForwardDirection );			// Gives Euler y-axis rotation for transform from forward direction vec2 in XZ-Plane
};