#pragma once
#include "AbilityData.hpp"
#include "Engine/Core/XMLUtilities.hpp"

AbilityData::AbilityData( const XMLElement& definitionElement )
{
	m_chanceToRun	= ParseXmlAttribute( definitionElement, "chanceToRun", m_chanceToRun );
	m_delay			= ParseXmlAttribute( definitionElement, "delay", m_delay );
	m_range			= ParseXmlAttribute( definitionElement, "range", m_range );
	m_jump			= ParseXmlAttribute( definitionElement, "jump", m_jump );
	m_cost			= ParseXmlAttribute( definitionElement, "cost", m_cost );
	m_aoeHorizontal	= ParseXmlAttribute( definitionElement, "aoe_horizontal", m_aoeHorizontal );
	m_aoeVertical	= ParseXmlAttribute( definitionElement, "aoe_vertical", m_aoeVertical );
	m_damage		= ParseXmlAttribute( definitionElement, "damage", 0.f );							// Damage and Heal: same thing with opposite sign

	if( m_damage == 0.f )																				// If there is no damage, search for heal
		m_damage	= -1.f * ParseXmlAttribute( definitionElement, "heal", 0.f );						// -1*heal will be resultant damage
}

AbilityData::~AbilityData()
{

}