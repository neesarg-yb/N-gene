#pragma once
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Game/GameCommon.hpp"

class Entity
{
public:
			 Entity( Clock *parentClock );
	virtual ~Entity();

public:
	virtual void Update();
	virtual void Render() const = 0;

public:
	ePhysicsMode m_physicsMode	= PHYSICS_WALK;

	Vector3		 m_position			= Vector3::ZERO;
	Vector3		 m_velocity			= Vector3::ZERO;
	Vector3		 m_willpowerForce	= Vector3::ZERO;		// Gets Zero(ed) out, at the end of each frame

public:
	float		 m_gravity			 = 9.8f;
	float		 m_frictionScale	 = 0.2f;

	float		 m_walkingMaxSpeedXY = 10.f;
	float		 m_flyingMaxSpeedXY	 = 10.f;

protected:
	Clock		 m_clock;
};
