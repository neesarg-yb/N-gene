#pragma once
#include "Engine/Math/Sphere.hpp"
#include "Game/World/Entity.hpp"

class World;

class Player : public Entity
{
public:
	 Player( Vector3 spawnPosition, World const *inTheWorld, Clock *parentClock );
	~Player();

public:
	World	const *m_world	= nullptr;
	Vector3 const  m_size	= Vector3( 1.f, 1.f, 1.8f );

public:
	bool m_inputControlsPlayer = false;

private:
	bool	m_isInAir			 = true;
	float	m_xyMovementStrength = 5.f;
	float	m_flyMovmentStrength = 10.f;

public:
	void Update();
	void Render() const;

	void UpdateIsInAir();

private:
	void SetWillpowerForceAccordingToInput();

public:
	bool	GetIsInAir() const;
	Sphere	GetCollider() const;
	Vector3	GetEyePosition() const;
	void	SetPositionFrom( Sphere const &fromCollider );
	
public:
	void AddWillpowerForceXY( Vector2 const &movementForce );		// XY Movement
	void AddWillPowerForceZ ( float const flyForce );				// Flying
};

inline bool Player::GetIsInAir() const 
{
	return m_isInAir;
}
