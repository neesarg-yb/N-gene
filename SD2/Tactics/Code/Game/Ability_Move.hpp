#pragma once
#include "Game/Ability.hpp"

class Ability_Move : public Ability
{
public:
	 Ability_Move( Actor& hostActor, std::string abilityName );
	~Ability_Move();

public:
	bool LetPlayerFinalizeTheAction_Update( bool& out_actionCancelled, float deltaSeconds );
	bool IsExecutingAction_Update( float deltaSeconds );

private:
	Vector3		m_moveToPosition = Vector3::ZERO;
	const float	m_totalLerpTime	 = 1.f;
};