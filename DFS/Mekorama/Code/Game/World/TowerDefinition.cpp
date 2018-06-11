#pragma once
#include "TowerDefinition.hpp"
#include "Engine/Core/StringUtils.hpp"

using namespace tinyxml2;

TowerDefinitionMap TowerDefinition::s_definitions;

TowerDefinition::TowerDefinition( XMLElement const &root )
{
	// Properties
	m_towerName		= ParseXmlAttribute( root, "name", m_towerName );
	m_xzDimension	= ParseXmlAttribute( root, "xzDimension", m_xzDimension );

	// For each slices
	for( XMLElement const *thisSlice = root.FirstChildElement("Slice"); thisSlice != nullptr; thisSlice = thisSlice->NextSiblingElement("Slice") )
	{
		// For each blocks
		for( XMLElement const *thisBlock = thisSlice->FirstChildElement("Block"); thisBlock != nullptr; thisBlock = thisBlock->NextSiblingElement("Block") )
		{
			std::string blockType = ParseXmlAttribute( *thisBlock, "type", "" );
			
			// Push it in the list
			m_blocksDefinitionList.push_back( blockType );
		}

	}
}

TowerDefinition::~TowerDefinition()
{

}

void TowerDefinition::LoadDefinition( std::string pathToDefinitionXML )
{
	// Get Root
	XMLDocument blockDoc;
	blockDoc.LoadFile( pathToDefinitionXML.c_str() );
	XMLElement* root = blockDoc.RootElement();

	std::string rootName = root->Name();
	GUARANTEE_OR_DIE( rootName == "TowerDefinition", Stringf( "Warning: Root of file \"%s\" is not \"TowerDefinition\"!", pathToDefinitionXML.c_str() ) );
	
	std::string	nameOfCurrDef = ParseXmlAttribute( *root, "name", std::string("") );
	TowerDefinitionMap::iterator it = s_definitions.find( nameOfCurrDef );

	// If this definition doesn't exist
	if( nameOfCurrDef != "" && it == s_definitions.end() )
	{
		TowerDefinition* newDefToPush = new TowerDefinition( *root );
		s_definitions[ nameOfCurrDef ] = newDefToPush;
	}
	else
		GUARANTEE_RECOVERABLE( false, Stringf( "TowerDefinition: \"%s\\, already exists in the pool!", nameOfCurrDef.c_str() ) );
}

void TowerDefinition::DeleteAllDefinitions()
{
	// Delete each
	for( TowerDefinitionMap::iterator it = s_definitions.begin(); it != s_definitions.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	// Clear the pool
	s_definitions.clear();
}