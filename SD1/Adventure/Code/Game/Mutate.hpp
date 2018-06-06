#pragma once
#include "Game/MapGenStep.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

class Mutate : public MapGenStep
{
public:
	 Mutate( const XMLElement& genStepXmlElement );
	~Mutate();

	TileDefinition*	m_ifTile	= nullptr;
	TileDefinition* m_toTile	= nullptr;
	float			m_chance	= 0.f;

	void Run( Map& map );

private:
};
