#pragma once
#include "Game/Actor.hpp"
#include "Game/AbilityData.hpp"

class Ability
{
public:
	Actor&			m_hostActor;
	std::string		m_abilityName;
	AbilityData*	m_abilityData		= nullptr;

public:
			 Ability( Actor& hostActor, std::string abilityName );		// NOTE! Use ConstructAbilityNamedForActor, instead.
	virtual ~Ability() { };

public:
	static  Ability*	ConstructAbilityNamedForActor( std::string abilityName, Actor& hostActor );
	virtual bool		LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds ) = 0;		// Returns true when the player has finalized the Action
	virtual bool		IsExecutingAction_Update( float deltaSeconds ) = 0;											// Returns true when the action has finished executing

public:
	static	float		GetTotalCostTimeForActions( const std::vector< Ability* > &actions );
};