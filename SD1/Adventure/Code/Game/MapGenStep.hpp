#pragma once
#include <string>
#include "Engine/../ThirdParty/tinyxml/tinyxml2.h"

class Map;

typedef tinyxml2::XMLElement XMLElement;
typedef tinyxml2::XMLAttribute XMLAttribute;

class MapGenStep
{
public:
	 MapGenStep( const XMLElement& genStepXmlElement );
	~MapGenStep();

	std::string		m_name			= "";
	int				m_iterations	= 1;
	float			m_chanceToRun	= 1.f;
	
	virtual void Run( Map& map ) = 0; // "pure virtual", MUST be overridden by subclasses

public:
	static MapGenStep* CreateMapGenStep( const XMLElement& genStepXmlElement );
};