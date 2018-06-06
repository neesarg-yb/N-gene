#pragma once

#include <map>
#include <string>
#include "Game/EntityDefinition.hpp"
#include "Engine/Core/XMLUtilities.hpp"

class PortalDefinition : public EntityDefinition
{
public:
	~PortalDefinition();
	static void LoadDefinitions( const XMLElement& root );

	static std::map< std::string, PortalDefinition* > s_definitions;

private:
	PortalDefinition( const XMLElement& root );
};