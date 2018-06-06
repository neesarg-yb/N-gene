#pragma once

#include <map>
#include <vector>
#include <string>
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/AIBehavior.hpp"

class ActorDefinition : public EntityDefinition
{
public:
	~ActorDefinition();
	static void LoadDefinitions( const XMLElement& root );

	Faction		m_faction		= Faction::UNKNOWN;
	float		m_startHealth	= 1.f;
	float		m_maxHealth		= 1.f;
	
	std::vector< AIBehavior* >						 m_AIBehaviors;
	static std::map< std::string, ActorDefinition* > s_definitions;

private:
	ActorDefinition( const XMLElement& root );
};