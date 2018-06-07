#pragma once
#include "Game/GameObject.hpp"

class Tank : public GameObject
{
public:
	 Tank( Vector3 const &spawnPosition );
	~Tank();

public:
	float m_speed = 10.f;			// units per seconds
	float m_rotationSpeed = 35.f;	// degrees per seconds
	
	// Debug Trail
	float m_spawnTrailPointAfter = 0.2f;

public:
	void Update( float deltaSeconds );
};