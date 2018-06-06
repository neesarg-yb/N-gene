#pragma once
#include "Projectile.hpp"
#include "Game/ProjectileDefinition.hpp"

Projectile::Projectile( Vector2& position, float shootDirectionInDegrees, std::string definitionName, Faction faction )
	:Entity( position, shootDirectionInDegrees, *ProjectileDefinition::s_definitions[ definitionName ] )
{
	m_faction = faction;
	SetVelocityDirectionInDegree( m_definition->m_maxSpeed, shootDirectionInDegrees );
}

void Projectile::Update( float deltaSeconds )
{
	Entity::Update(deltaSeconds);

	// If health is zero, it should die
	if( m_health <= 0.f )
		m_readyToDie = true;
}

void Projectile::Render()
{
	Entity::Render();
}