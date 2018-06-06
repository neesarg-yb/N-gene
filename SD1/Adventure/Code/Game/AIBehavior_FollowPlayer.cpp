#pragma once

#include "AIBehavior_FollowPlayer.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Engine/Math/HeatMap.hpp"

AIBehavior_FollowPlayer::AIBehavior_FollowPlayer( const XMLElement& followBehaviourElement )
	: AIBehavior( followBehaviourElement )
{
}

AIBehavior_FollowPlayer::~AIBehavior_FollowPlayer()
{

}

void AIBehavior_FollowPlayer::Update( float deltaSeconds )
{
	// TODO: Use Raycast for visibility
	UNUSED( deltaSeconds );

	// TODO: Implement the behavior
}

AIBehavior* AIBehavior_FollowPlayer::CloneMeForActor( Actor& hostActor ) const
{
	AIBehavior* clone = new AIBehavior_FollowPlayer( *this );
	clone->m_hostActor = &hostActor;

	return clone;
}

float AIBehavior_FollowPlayer::CalculateUtility( Map& currentMap, Player& currentPlayer )
{
	UNUSED( currentMap );
	UNUSED( currentPlayer );

	return m_utility;
}