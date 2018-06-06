#pragma once
#include "Ability_Fireball.hpp"

Ability_Fireball::Ability_Fireball( Actor& hostActor, std::string abilityName )
	: Ability( hostActor, abilityName )
{

}

Ability_Fireball::~Ability_Fireball()
{

}

bool Ability_Fireball::LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds )
{
	UNUSED( deltaSeconds );
	out_actionCancelled = false;

	return true;
}

bool Ability_Fireball::IsExecutingAction_Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	return true;
}