#pragma once
#include "Game/Ability.hpp"

class Ability_Heal : public Ability
{
public:
	 Ability_Heal( Actor& hostActor, std::string abilityName );
	~Ability_Heal();

public:
	bool LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds );
	bool IsExecutingAction_Update( float deltaSeconds );

};