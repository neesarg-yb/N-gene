#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"

enum CurrentPlayerState
{
	MENU_SELECTION = 0,
	MOVE,
	ATTACK,
	WAIT,
	NUM_CURRENT_PLAYER_STATE
};

class Battle;

class BattleState
{
	friend Battle;

public:
	 BattleState( Map* currentMap );
	~BattleState();

public:
	Map*					m_currentMap;
	std::vector< Actor* >	m_players;
	Actor*					m_currentTurnOf;
	CurrentPlayerState		m_currentPlayerState	= MENU_SELECTION;

	void		AddNewPlayer( ActorDefinition& actorDefinition );

private:
	void		ChangeTurnOfPlayer();
	void		ProceedTheGameByTime( int timeIncrement );
	int			GetMutualWaitingTime() const;						// returns zero if any one of the players have waiting time in negative
	int			GetWaitingTimeToSkipTurn() const;
	void		AttackTowardsBlockSituatedAt( int x, int z );
};