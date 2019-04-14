#pragma once
#include "Engine/Math/Sphere.hpp"
#include "Game/World/Entity.hpp"

class Player : public Entity
{
public:
	 Player( Vector3 spawnPosition, Clock *parentClock );
	~Player();

private:
	bool	m_updateEnabled	 = false;
	Vector3	m_size			 = Vector3( 1.f, 1.f, 1.8f );

public:
	void Update();
	void Render() const;

public:
	Sphere	GetCollider() const;
	void	SetPositionFrom( Sphere const &fromCollider );

private:
	bool DebugPausePhysics();			// Returns true if supposed to be paused
	void SetWillpowerAndStrengths();	// XY & Fly Movement
};


