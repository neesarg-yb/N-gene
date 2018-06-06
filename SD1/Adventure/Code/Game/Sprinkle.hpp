#pragma once
#include "Game/MapGenStep.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/TileDefinition.hpp"

class Sprinkle : public MapGenStep
{
public:
	 Sprinkle( const XMLElement& genStepXmlElement );
	~Sprinkle();

	TileDefinition* m_sprinkleTile	= nullptr;
	float			m_fraction		= 0.15f;
	int				m_edgeThickness	= 1;

	void Run( Map& map );

private:
};
