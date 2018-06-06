#pragma once
#include "Engine/Core/Image.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/MapGenStep.hpp"
#include "Game/TileDefinition.hpp"

class FromFile : public MapGenStep
{
public:
	 FromFile( const XMLElement& genStepXmlElement );
	~FromFile();

	std::string		m_imageName		= "";
	Image*			m_sourceImage	= nullptr;

	void Run( Map& map );

private:
};
