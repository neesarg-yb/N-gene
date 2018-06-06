#pragma once

#include <map>
#include <string>
#include "Game/EntityDefinition.hpp"
#include "Engine/Core/XMLUtilities.hpp"

class ProjectileDefinition : public EntityDefinition
{
public:
	~ProjectileDefinition();
	static void LoadDefinitions( const XMLElement& root );

	float m_damageAmount	= 1.f;
	static std::map< std::string, ProjectileDefinition* > s_definitions;

private:
	ProjectileDefinition( const XMLElement& root );
};