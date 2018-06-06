#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/UIMenu.hpp"
#include "Engine/Math/Transform.hpp"
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/SpriteAndAnimation/Animator.hpp"

class Ability;

enum ActorTurnState
{
	ACTION_SELECTION = 0,
	ACTION_FINALIZING
};

class Actor
{
public:
	 Actor( Map& currentMap, Vector3 position, ActorDefinition* actorDefinition );
	~Actor();

public:
	ActorDefinition*	m_definition;
	Map&				m_currentMap;
	Vector3				m_position;
	Vector3				m_facingTowardsDirection = Vector3( 0.f, 0.f, 1.f);

	float				m_currentHealth			= 1.f;
	float				m_waitingTime			= 60;
	int					m_costToMove			= 30;
	int					m_costToAttack			= 60;
	int					m_costToWait			= 20;
	const int			m_totalActionsAllowed	= 2;

public:
	void		Update( float deltaSeconds );													// It should be called every frame, before IsTakingTurn( ) gets called
	void		Render() const;
	Ability*	IsTakingTurn_Update( float deltaSeconds, bool& out_finishedTakingTurn );		// Note: Call Update( ) before calling this function.. \n out_finishedTakingTurn is set true when he's done taking turn \n Returns newly created Action or nullptr
	void		IsTakingTurn_Render() const;													// Renders turn specific stuffs like ActionsUIMenu, etc..

	Vector3		GetPosition() const;
	void		SetPosition( Vector3 const & newPosition );

	void		IncreaseWaitingTimeBy( float timeIncrease = 0.f );
	void		JustAttacked();
	void		JustMoved();
	void		StartedWaiting( int canWaitUpto = -1 );

	bool		IsReadyForTurn() const;
	std::vector< IntVector2 >	CanMoveToLocationsAccordingToHeatMap() const;

private:
	Animator*		m_animator						= nullptr;
	ActorTurnState	m_currentTurnState				= ACTION_SELECTION;
	std::string		m_selectedActionName			= "";
	UIMenu*			m_actionsMenu					= nullptr;
	ShaderProgram*	m_ignoreAlphaShaderProgram		= nullptr;

	std::function< void( const char* ) > callBack = std::bind( &Actor::ActionSelected, this, std::placeholders::_1 );

	void	ActionSelected( const char* actionName );
	void	ResetActionSelected();
};