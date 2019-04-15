#pragma once
#include "Engine/Math/Sphere.hpp"
#include "Game/World/Entity.hpp"

class Player : public Entity
{
public:
	 Player( Vector3 spawnPosition, Clock *parentClock );
	~Player();

public:
	Vector3 const m_size = Vector3( 1.f, 1.f, 1.8f );

public:
	void Update();
	void Render() const;

public:
	Sphere	GetCollider() const;
	Vector3	GetEyePosition() const;
	void	SetPositionFrom( Sphere const &fromCollider );
	
public:
	void SetMovementWillpowerAndStrength( Vector2 const &xyMovementIntention, float strength );		// XY Movement
	void SetFlyWillpowerAndStrength( float const flyIntention, float strength );					// Fly
};


