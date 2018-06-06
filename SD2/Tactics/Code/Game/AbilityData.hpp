#pragma once
#include "Game/GameCommon.hpp"

class AbilityData
{
public:
	 AbilityData( const XMLElement& definitionElement );
	~AbilityData();

public:
	float	m_chanceToRun	= 1.f;
	float	m_delay			= 0.f;

	int		m_range			= 0;
	int		m_jump			= 0;
	float	m_damage		= 0.f;
	float	m_cost			= 0.f;
	float	m_aoeHorizontal	= 0.f;
	float	m_aoeVertical	= 0.f;

private:
};