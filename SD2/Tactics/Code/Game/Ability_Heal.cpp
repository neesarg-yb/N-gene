#pragma once
#include "Ability_Heal.hpp"

Ability_Heal::Ability_Heal( Actor& hostActor, std::string abilityName )
	: Ability( hostActor, abilityName )
{

}

Ability_Heal::~Ability_Heal()
{

}

bool Ability_Heal::LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds )
{
	UNUSED( deltaSeconds );
	out_actionCancelled = false;

	return true;
}

bool Ability_Heal::IsExecutingAction_Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	return true;
}