#pragma once
#include "Game/Ability.hpp"

class Ability_Attack : public Ability
{
public:
	 Ability_Attack( Actor& hostActor, std::string abilityName );
	~Ability_Attack();

public:
	bool LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds );
	bool IsExecutingAction_Update( float deltaSeconds );

private:
	Vector3 m_attackToPosition = Vector3::ZERO;
};