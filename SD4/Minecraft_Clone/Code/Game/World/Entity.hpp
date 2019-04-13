#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Game/GameCommon.hpp"

class Entity
{
public:
	 Entity();
	~Entity();

public:
	void Update();
	void Render() const;

public:
	ePhysicsMode m_physicsMode	= PHYSICS_WALK;

	Vector3		 m_position		= Vector3::ZERO;
	Vector3		 m_velocity		= Vector3::ZERO;
	Vector3		 m_willpowerMoveIntention = Vector3::ZERO;

public:
	float		 m_groundFriction	 = 0.f;
	float		 m_walkingMaxSpeedXY = 0.f;
	float		 m_flyingMaxSpeedXY	 = 0.f;
};
