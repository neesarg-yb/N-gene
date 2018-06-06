#pragma once
#include "Engine/Input/Command.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/UIMenu.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Game/Block.hpp"
#include "Game/Actor.hpp"

class Ability;
class Ability_Attack;
class Ability_Defend;
class Ability_Fireball;
class Ability_Heal;
class Ability_Move;
class Ability_ShootArrow;


class Battle
{
	friend Actor;
	friend Ability;
	friend Ability_Attack;
	friend Ability_Defend;
	friend Ability_Fireball;
	friend Ability_Heal;
	friend Ability_Move;
	friend Ability_ShootArrow;

public:
	 Battle();
	~Battle();

	void Update( float deltaSeconds );
	void Render() const;

	void AddNewPlayer( ActorDefinition& actorDefinition );
	void DeleteAllPlayers();

	OrbitCamera*			m_camera					= nullptr;
private:
	double					m_timeSinceStartOfTheBattle	= 0;
	Map*					m_currentMap				= nullptr;
	Tile*					m_indicatorTile				= nullptr;
	std::vector< Actor* >	m_players;
	Actor*					m_currentPlayer				= nullptr;
	std::vector< Ability* >	m_actionsQueue;


private:
	double	GetTimeSinceBattleStarted() const;

	void	MoveIndicatorWithLeftStick	( float deltaSeconds );
	void	RotateCameraWithRightStick	( float deltaSeconds );
	void	ZoomTheCameraBasedOnTriggers( float deltaSeconds );
	void	MoveCurrentPlayerOnTheIndicator();
	void	MoveIndicatorToCurrentPlayerPosition();
	void	FocusCameraOnCurrentPlayer	( float deltaSeconds );
	void	FocusCameraOnIndicatorTile	( float deltaSeconds );

	void	ProceedTheGameWithWaitTimeReduction( float totalTimeReduction );		// Increases waitTime for currentPlayer; Decreases the same for all other players
	void	CheckAndChangeThePlayerTurn();											// Gives the turn to the player with least waitTime
};