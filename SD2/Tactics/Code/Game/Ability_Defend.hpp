#pragma once
#include "Game/Ability.hpp"

class Ability_Defend : public Ability
{
public:
	 Ability_Defend( Actor& hostActor, std::string abilityName );
	~Ability_Defend();

public:
	bool LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds );
	bool IsExecutingAction_Update( float deltaSeconds );

};