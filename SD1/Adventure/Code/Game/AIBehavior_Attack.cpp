#pragma once
#include "AIBehavior_Attack.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Player.hpp"

AIBehavior_Attack::AIBehavior_Attack( const XMLElement& followBehaviourElement )
	: AIBehavior( followBehaviourElement )
{
	m_range			= ParseXmlAttribute( followBehaviourElement, "range", m_range );
	m_rateOfFire	= ParseXmlAttribute( followBehaviourElement, "rateOfFire", m_rateOfFire );
}

AIBehavior_Attack::~AIBehavior_Attack()
{

}

void AIBehavior_Attack::Update( float deltaSeconds )
{
	// TODO: Use Raycast for visibility
	UNUSED( deltaSeconds );

	// FOLLOW THE PLAYER
	m_goalPosition = m_currentPlayer->m_position;

	// Fire a projectile towards Player
	Projectile* fireProjectile = m_hostActor->CreateAProjectile( m_rateOfFire );
	if( fireProjectile != nullptr )
	{
		m_currentMap->AddEntityToList( fireProjectile, ENTITY_TYPE_PROJECTILE );
	}
	
	// Get goal direction
	Vector2 directionTowardsGoalPos = m_goalPosition - m_hostActor->m_position;
	directionTowardsGoalPos.NormalizeAndGetLength();

	// Putting if condition, because getting to precise location with direction is very very rare..
	if( Vector2::GetDistance( m_goalPosition, m_hostActor->m_position) < 0.5f )
		directionTowardsGoalPos = Vector2::ZERO;

	// Move the Goblin, accordingly
	m_hostActor->m_velocity = directionTowardsGoalPos * m_hostActor->m_definition->m_maxSpeed;
}

AIBehavior* AIBehavior_Attack::CloneMeForActor( Actor& hostActor ) const
{
	AIBehavior* clone = new AIBehavior_Attack( *this );
	clone->m_hostActor = &hostActor;

	return clone;
}

float AIBehavior_Attack::CalculateUtility( Map& currentMap, Player& currentPlayer )
{
	m_currentMap = &currentMap;
	m_currentPlayer = &currentPlayer;

	float distToPlayer = Vector2::GetDistance( m_hostActor->m_position, currentPlayer.m_position );
	// Calculate utility based on shootingRate
	if( m_hostActor->m_timeElaspedSinceLastShooting > (1.f / m_rateOfFire)							// If you can shoot according to m_rateOfFire
		&& distToPlayer <= m_range )																	// If distance is in m_range
		m_utility = 0.6f;
	else
		m_utility = 0.f;

	return m_utility;
}