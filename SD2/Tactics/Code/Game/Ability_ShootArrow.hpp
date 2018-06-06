#pragma once
#include "Game/Ability.hpp"

class Ability_ShootArrow : public Ability
{
public:
	 Ability_ShootArrow( Actor& hostActor, std::string abilityName );
	~Ability_ShootArrow();

public:
	bool LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds );
	bool IsExecutingAction_Update( float deltaSeconds );
};