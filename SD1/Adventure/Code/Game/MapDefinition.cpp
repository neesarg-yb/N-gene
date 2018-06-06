#pragma once
#include "MapDefinition.hpp"
#include "Engine/Math/MathUtil.hpp"

std::map< std::string, MapDefinition* >	MapDefinition::s_definitions;

MapDefinition::MapDefinition()
{

}

MapDefinition::MapDefinition( const XMLElement& root )
{
	std::string nameOfRoot = root.Name();
	GUARANTEE_RECOVERABLE( nameOfRoot == "MapDefinitions", "Root element of Maps.xml should be MapDefinitions..!");


	for( XMLElement* thisDefinition = (XMLElement*) root.FirstChildElement(); thisDefinition != nullptr; thisDefinition = thisDefinition->NextSiblingElement() )
	{
		// Check if current element is MapDefinition or not
		GUARANTEE_RECOVERABLE( std::string( thisDefinition->Name() ) == "MapDefinition", "XML Element named wrong( " + std::string( thisDefinition->Name() ) + " ). It should be named MapDefinition..!" );

		std::string	nameOfCurrDef = ParseXmlAttribute( *thisDefinition, "name", std::string("") );
		std::map< std::string, MapDefinition* >::iterator it = s_definitions.find( nameOfCurrDef );

		// If this definition doesn't exist
		if( nameOfCurrDef != "" && it == s_definitions.end() )
		{
			// Creates new mapDefinition and adds it to s_definitions
			MapDefinition* newDef = new MapDefinition();

			newDef->m_defaultTile		= ParseXmlAttribute( *thisDefinition, "defaultTile", m_defaultTile );
			IntRange widthRange			= ParseXmlAttribute( *thisDefinition, "width", widthRange );
			IntRange heightRange		= ParseXmlAttribute( *thisDefinition, "height", heightRange );
			newDef->m_width				= GetRandomIntInRange( widthRange.min, widthRange.max );
			newDef->m_height			= GetRandomIntInRange( heightRange.min, heightRange.max );
			newDef->m_definitionName	= nameOfCurrDef;

			// For every GenerationSteps, sets pointer to its MapGenStep
			for( XMLElement* thisChild = (XMLElement*) thisDefinition->FirstChildElement()->FirstChildElement(); thisChild != nullptr; thisChild = thisChild->NextSiblingElement() )
			{
				MapGenStep* newGenStep = MapGenStep::CreateMapGenStep( *thisChild );
				std::string newGenStepName = thisChild->Name();

				if( newGenStep != nullptr)
				{
					// genStep found, add it to the vector
					newDef->m_mapGenSteps.push_back( newGenStep );
				}
				else
				{
					// Show Error, no matching MapGenStep found
					GUARANTEE_OR_DIE( false, "MapGenStep " + newGenStepName + " not found..!" );
				}
			}

			// Add new definition to static vector..
			s_definitions[ newDef->m_definitionName ] = newDef;
		}
	}
}

MapDefinition::~MapDefinition()
{

}

void MapDefinition::LoadMapDefinitions( const XMLElement& root )
{
	new MapDefinition( root );
}