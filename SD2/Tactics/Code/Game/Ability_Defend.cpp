#pragma once
#include "Ability_Defend.hpp"

Ability_Defend::Ability_Defend( Actor& hostActor, std::string abilityName )
	: Ability( hostActor, abilityName )
{

}

Ability_Defend::~Ability_Defend()
{

}

bool Ability_Defend::LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds )
{
	UNUSED( deltaSeconds );
	out_actionCancelled = false;

	return true;
}

bool Ability_Defend::IsExecutingAction_Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	return true;
}