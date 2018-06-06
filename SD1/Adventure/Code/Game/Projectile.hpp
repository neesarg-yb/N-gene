#pragma once
#include "Game/Entity.hpp"

class Projectile : public Entity
{
public:
	Projectile( Vector2& position, float shootDirectionInDegrees, std::string definitionName, Faction faction );

	float	m_health	= 1.f;
	Faction	m_faction	= Faction::UNKNOWN;

	void Update( float deltaSeconds );
	void Render();
private:

};