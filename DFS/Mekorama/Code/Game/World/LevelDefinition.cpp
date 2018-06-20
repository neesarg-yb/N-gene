#pragma once
#include "LevelDefinition.hpp"
#include "Engine/Core/StringUtils.hpp"

using namespace tinyxml2;

LevelDefinitionMap LevelDefinition::s_definitions;

LevelDefinition::LevelDefinition( XMLElement const &root )
{
	// Get all the elements
	XMLElement const *towerElement	= root.FirstChildElement( "PuzzleTower" );
	XMLElement const *spawnElement	= root.FirstChildElement( "SpawnPlayer" );
	XMLElement const *finishElement	= root.FirstChildElement( "FinishPoint" );
	
	// Tower Name
	m_towerName			= ParseXmlAttribute( *towerElement, "name", m_towerName );
	
	// Start Position
	IntVector2	xzPos	= IntVector2( m_startPosition.x, m_startPosition.z );
	int			yPos	= m_startPosition.y;

	xzPos				= ParseXmlAttribute( *spawnElement, "onBlock", xzPos );
	yPos				= ParseXmlAttribute( *spawnElement, "inLayer", yPos );
	yPos++;				// It is onBlock of that layer; i.e. same xzCoord but a layer higher

	m_startPosition		= IntVector3( xzPos.x, yPos, xzPos.y );

	// Finish Position
	xzPos				= IntVector2( m_finishPosition.x, m_finishPosition.z );
	yPos				= m_finishPosition.y;

	xzPos				= ParseXmlAttribute( *finishElement, "atBlock", xzPos );
	yPos				= ParseXmlAttribute( *finishElement, "inLayer", yPos );

	m_finishPosition	= IntVector3( xzPos.x, yPos, xzPos.y );
}

LevelDefinition::~LevelDefinition()
{

}

void LevelDefinition::LoadDefinition( std::string pathToDefinitionXML )
{
	// Get Root
	XMLDocument blockDoc;
	blockDoc.LoadFile( pathToDefinitionXML.c_str() );
	XMLElement* root = blockDoc.RootElement();

	std::string rootName = root->Name();
	GUARANTEE_OR_DIE( rootName == "LevelDefinition", Stringf( "Warning: Root of file \"%s\" is not \"LevelDefinition\"!", pathToDefinitionXML.c_str() ) );

	std::string	nameOfCurrDef = ParseXmlAttribute( *root, "name", std::string("") );
	LevelDefinitionMap::iterator it = s_definitions.find( nameOfCurrDef );

	// If this definition doesn't exist
	if( nameOfCurrDef != "" && it == s_definitions.end() )
	{
		LevelDefinition* newDefToPush = new LevelDefinition( *root );
		s_definitions[ nameOfCurrDef ] = newDefToPush;
	}
	else
		GUARANTEE_RECOVERABLE( false, Stringf( "LevelDefinition: \"%s\\, already exists in the pool!", nameOfCurrDef.c_str() ) );
}

void LevelDefinition::DeleteAllDefinitions()
{
	// Delete each
	for( LevelDefinitionMap::iterator it = s_definitions.begin(); it != s_definitions.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	// Clear the pool
	s_definitions.clear();
}

