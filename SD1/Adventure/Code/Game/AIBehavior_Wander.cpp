#pragma once
#include "AIBehavior_Wander.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"

AIBehavior_Wander::AIBehavior_Wander( const XMLElement& wanderElement )
	: AIBehavior( wanderElement )
{
	m_proximity = ParseXmlAttribute( wanderElement, "proximity", m_proximity );

	m_goalPosition = Vector2( (float)GetRandomFloatInRange( -100, 100 ) , (float)GetRandomFloatInRange( -100, 100 ) );
}

AIBehavior_Wander::~AIBehavior_Wander()
{
	AIBehavior::~AIBehavior();
}

void AIBehavior_Wander::Update( float deltaSeconds )
{
	// WANDERING BEHAVIOUR
	m_timeElaspedSinceLastGoalChanged += deltaSeconds;
	if( m_timeElaspedSinceLastGoalChanged >= m_goalPositionChangeAfterSeconds )
	{
		IntVector2 lastTile = IntVector2( m_currentMap->m_dimension.x - 1 , m_currentMap->m_dimension.y - 1);
		int maxIndexOfTiles = m_currentMap->GetIndexOfTileAt( lastTile );

		IntVector2 randomTargetPosition = m_currentMap->m_tiles[ GetRandomNonNegativeIntLessThan( maxIndexOfTiles ) ].m_tileCoords;
		m_goalPosition = Vector2( (float)randomTargetPosition.x , (float)randomTargetPosition.y );

		m_timeElaspedSinceLastGoalChanged = 0.f;
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

AIBehavior* AIBehavior_Wander::CloneMeForActor( Actor& hostActor ) const
{
	AIBehavior* clone = new AIBehavior_Wander( *this );
	clone->m_hostActor = &hostActor;

	return clone;
}

float AIBehavior_Wander::CalculateUtility( Map& currentMap, Player& currentPlayer )
{
	m_utility = 0.2f;

	m_currentMap = &currentMap;
	UNUSED( currentPlayer );

	return m_utility;
}