#pragma once
#include "Game/GameObject.hpp"
#include "Engine/Math/Vector3.hpp"

class Bullet : public GameObject
{
public:
	 Bullet( Vector3 const &spawnPosition, Vector3 const &velocity );
	~Bullet();

public:
	Vector3		m_velocity		= Vector3::ZERO;	// per second
	float		m_bulletSpeed	= 25.f;				// units per seconds

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
};