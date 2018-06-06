#pragma once
#include "Game/Ability.hpp"

class Ability_Fireball : public Ability
{
public:
	 Ability_Fireball( Actor& hostActor, std::string abilityName );
	~Ability_Fireball();

public:
	bool LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds );
	bool IsExecutingAction_Update( float deltaSeconds );

};