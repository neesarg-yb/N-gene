#pragma once
#include "Ability.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Ability_Move.hpp"
#include "Game/Ability_Attack.hpp"
#include "Game/Ability_ShootArrow.hpp"
#include "Game/Ability_Fireball.hpp"
#include "Game/Ability_Defend.hpp"
#include "Game/Ability_Heal.hpp"

Ability::Ability( Actor& hostActor, std::string abilityName )
	: m_hostActor( hostActor )
	, m_abilityName( abilityName )
{
	m_abilityData = m_hostActor.m_definition->m_dataOfAbilities[ m_abilityName ];
}

Ability* Ability::ConstructAbilityNamedForActor( std::string abilityName, Actor& hostActor )
{
	Ability* toReturn = nullptr;

	if ( abilityName == "Move" )
		toReturn = new Ability_Move( hostActor, abilityName );
	else if ( abilityName == "Attack" )
		toReturn = new Ability_Attack( hostActor, abilityName );
	else if ( abilityName == "ShootArrow" )
		toReturn = new Ability_ShootArrow( hostActor, abilityName );
	else if ( abilityName == "Fireball" )
		toReturn = new Ability_Fireball( hostActor, abilityName );
	else if ( abilityName == "Defend" )
		toReturn = new Ability_Defend( hostActor, abilityName );
	else if ( abilityName == "Heal" )
		toReturn = new Ability_Heal( hostActor, abilityName );

	GUARANTEE_RECOVERABLE( toReturn != nullptr, std::string("FactoryFunction: Ability_" + std::string(abilityName) + ", not found!!") );

	return toReturn;
}

float Ability::GetTotalCostTimeForActions( const std::vector< Ability* > &actions )
{
	float totalCostTime = 0.f;

	for each ( Ability* action in actions )
		totalCostTime += action->m_abilityData->m_cost;

	return totalCostTime;
}