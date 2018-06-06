#pragma once
#include "Ability_ShootArrow.hpp"

Ability_ShootArrow::Ability_ShootArrow( Actor& hostActor, std::string abilityName )
	: Ability( hostActor, abilityName )
{

}

Ability_ShootArrow::~Ability_ShootArrow()
{

}

bool Ability_ShootArrow::LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds )
{
	UNUSED( deltaSeconds );
	out_actionCancelled = false;

	return true;
}

bool Ability_ShootArrow::IsExecutingAction_Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	return true;
}