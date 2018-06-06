#pragma once
#include "BattleState.hpp"

BattleState::BattleState( Map* currentMap )
	: m_currentMap( currentMap )
{
	m_currentTurnOf = nullptr;
}

BattleState::~BattleState()
{
	for( int i = 0; i < m_players.size(); )
	{
		delete m_players[i];
		m_players.erase( m_players.begin() + i );
	}

	delete m_currentMap;
}

void BattleState::AddNewPlayer( ActorDefinition& actorDefinition )
{
	int xSpawnPos = GetRandomIntInRange( 0, m_currentMap->m_dimensions.x - 1 );
	int zSpawnPos = GetRandomIntInRange( 0, m_currentMap->m_dimensions.z - 1 );
	
	Vector3 worldPos = m_currentMap->GiveWorldPositionToRenderOnTopOfTheBlockAt( xSpawnPos, zSpawnPos );
	Actor* newPlayer = new Actor( *m_currentMap, worldPos, &actorDefinition );
	m_players.push_back( newPlayer );

	if( m_currentTurnOf == nullptr )
		m_currentTurnOf = newPlayer;
}

void BattleState::ChangeTurnOfPlayer()
{
	// TODO: TURN ( Move & Attack, both )
	if( m_currentTurnOf->m_waitingTime <= 0 )
		return;

	// Get the player having the lowest waiting time. i.e. either +ve or -ve
	Actor * playerWithLowestWaitingTime = nullptr;

	for( int i = 0; i < m_players.size(); i++ )
	{
		if( playerWithLowestWaitingTime == nullptr )
			playerWithLowestWaitingTime = m_players[i];

		if( m_players[i]->m_waitingTime < playerWithLowestWaitingTime->m_waitingTime )
			playerWithLowestWaitingTime = m_players[i];
	}
	
	// If there are no players in vector, this may occur
	if( playerWithLowestWaitingTime == nullptr )
		return;

	// If it can't take the turn
	if( playerWithLowestWaitingTime->m_waitingTime > 0 )
	{
		// Proceed game in fast forward..
		ProceedTheGameByTime( playerWithLowestWaitingTime->m_waitingTime );
	}

	// Now, Assign appropriate player to take turn
	m_currentTurnOf = playerWithLowestWaitingTime;
	
	m_currentPlayerState = MENU_SELECTION;
}

void BattleState::ProceedTheGameByTime( int timeIncrement )
{
	for each (Actor* player in m_players)
		player->IncreaseWaitingTimeBy( -timeIncrement );
}

int BattleState::GetMutualWaitingTime() const
{
	int mutualWaitingTime = INT_MAX;
	
	for( int i = 0; i < m_players.size(); i++ )
	{
		if( m_players[i]->m_waitingTime < mutualWaitingTime )
			mutualWaitingTime = m_players[i]->m_waitingTime;
	}
	
	return mutualWaitingTime > 0 ? mutualWaitingTime : 0;
}

int BattleState::GetWaitingTimeToSkipTurn() const
{
	// Get the player having the lowest waiting time. i.e. either +ve or -ve
	Actor * otherPlayerWithLowestWaitingTime = nullptr;

	for( int i = 0; i < m_players.size(); i++ )
	{
		if( otherPlayerWithLowestWaitingTime == nullptr )
			otherPlayerWithLowestWaitingTime = m_players[i];

		// otherPlayer should not be currentPlayer
		if( m_currentTurnOf != m_players[i] )	// See if otherPlayer have lower waiting time
			if( m_players[i]->m_waitingTime < otherPlayerWithLowestWaitingTime->m_waitingTime )
				otherPlayerWithLowestWaitingTime = m_players[i];
	}

	// Exception in case of no players
	if( otherPlayerWithLowestWaitingTime == nullptr )
		return 0;

	return (otherPlayerWithLowestWaitingTime->m_waitingTime <= 0) ? 0 : otherPlayerWithLowestWaitingTime->m_waitingTime;
}

void BattleState::AttackTowardsBlockSituatedAt( int x, int z )
{
	Vector3 attackingAt = m_currentMap->GiveWorldPositionToRenderOnTopOfTheBlockAt( x, z );
	Actor*  playerTakingDamage = nullptr;

	for each (Actor* player in m_players)
	{
		if( player->m_position == attackingAt )
			playerTakingDamage = player;
	}

	if( playerTakingDamage != nullptr )
	{
		playerTakingDamage->m_currentHealth -= 1.f;

		if( playerTakingDamage->m_currentHealth <= 0.f )
			ERROR_RECOVERABLE("You killed me.. :(\nRestart the game..");
	}
}