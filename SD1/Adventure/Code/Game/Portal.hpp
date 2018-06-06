#pragma once
#include <string>
#include "Engine/Math/Vector2.hpp"
#include "Game/Entity.hpp"
#include "Game/PortalDefinition.hpp"

class Map;

class Portal: public Entity
{
public:
	 Portal( Vector2& position, std::string portalType, std::string destinationMap, std::string reciprocalPortalType );
	~Portal();

	bool		m_isDisabled		= false;

	Portal*		m_reciprocalPortal	= nullptr;
	Map*		m_destinationMap	= nullptr;

	std::string m_portalType;
	std::string m_destinationMapName;
	std::string m_reciprocalPortalType;

	void Update( float deltaSeconds );
	void Render();

	Vector2 PrepPortalForPlayerSpawn();				// Gives the Portal's position & Disable the Portal after player is spawned on it.

private:

};