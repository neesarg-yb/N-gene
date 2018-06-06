#pragma once

#include "AIBehavior_Follow.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"

AIBehavior_Follow::AIBehavior_Follow( const XMLElement& followBehaviourElement )
	: AIBehavior( followBehaviourElement )
{
	m_range = ParseXmlAttribute( followBehaviourElement, "range", m_range );

}

AIBehavior_Follow::~AIBehavior_Follow()
{

}

void AIBehavior_Follow::Update( float deltaSeconds )
{
	// TODO: Use Raycast for visibility
	UNUSED( deltaSeconds );

	// FOLLOW THE PLAYER
	m_goalPosition = m_currentPlayer->m_position;

	// Get goal direction
	Vector2 directionTowardsGoalPos = m_goalPosition - m_hostActor->m_position;
	directionTowardsGoalPos.NormalizeAndGetLength();

	// Putting if condition, because getting to precise location with direction is very very rare..
	if( Vector2::GetDistance( m_goalPosition, m_hostActor->m_position) < 0.5f )
		directionTowardsGoalPos = Vector2::ZERO;

	// Move the Goblin, accordingly
	m_hostActor->m_velocity = directionTowardsGoalPos * m_hostActor->m_definition->m_maxSpeed;
}

AIBehavior* AIBehavior_Follow::CloneMeForActor( Actor& hostActor ) const
{
	AIBehavior* clone = new AIBehavior_Follow( *this );
	clone->m_hostActor = &hostActor;

	return clone;
}

float AIBehavior_Follow::CalculateUtility( Map& currentMap, Player& currentPlayer )
{
	m_currentMap = &currentMap;
	m_currentPlayer = &currentPlayer;

	float distToPlayer = Vector2::GetDistance( m_hostActor->m_position, currentPlayer.m_position );
	// Calculate utility based on distance
	if( distToPlayer <= m_range )
		m_utility = 0.5f;
	else
		m_utility = 0.f;

	return m_utility;
}