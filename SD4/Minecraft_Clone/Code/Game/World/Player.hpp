#pragma once
#include "Game/World/Entity.hpp"

class Player : public Entity
{
public:
	 Player( Vector3 spawnPosition, Clock *parentClock );
	~Player();

public:
	void Update();
	void Render() const;

private:
	Vector3 m_size = Vector3( 1.f, 1.f, 1.8f );
};
