#pragma once
#include "Game/MapGenStep.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/TileDefinition.hpp"

class FillAndEdge : public MapGenStep
{
public:
	 FillAndEdge( const XMLElement& genStepXmlElement );
	~FillAndEdge();

	TileDefinition* m_fillTileDef	= nullptr;
	TileDefinition* m_edgeTileDef	= nullptr;
	int				m_edgeThickness	= 1;

	void Run( Map& map );

private:
};
