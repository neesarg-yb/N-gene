#pragma once
#include "TowerDefinition.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVector3.hpp"

using namespace tinyxml2;

TowerDefinitionMap TowerDefinition::s_definitions;

TowerDefinition::TowerDefinition( XMLElement const &root )
{
	// Properties
	m_towerName		= ParseXmlAttribute( root, "name", m_towerName );
	m_xzDimension	= ParseXmlAttribute( root, "xzDimension", m_xzDimension );

	// For each slices/layers
	int sliceIndex = 0;
	for( XMLElement const *thisSlice = root.FirstChildElement("Layer"); thisSlice != nullptr; thisSlice = thisSlice->NextSiblingElement("Layer") )
	{
		// For each pipes
		XMLElement const *pipesElement = thisSlice->FirstChildElement("Pipes");
		if( pipesElement != nullptr )
		{
			for( XMLElement const *thisPipe = pipesElement->FirstChildElement("Pipe"); thisPipe != nullptr; thisPipe = thisPipe->NextSiblingElement("Pipe") )
			{
				IntVector2				xzPos		= ParseXmlAttribute( *thisPipe, "startPos", IntVector2::ZERO );
				ePipeForwardDirection	forwardDir	= ParseXmlAttributeForPipeDirection( *thisPipe, "towards", PIPE_DIR_WORLD_FRONT );
				float					length		= ParseXmlAttribute( *thisPipe, "length", 1.f );
				IntVector3				xyzPos		= IntVector3( xzPos.x, sliceIndex, xzPos.y );

				PipeSpawnData			spawnData	= PipeSpawnData( xyzPos, forwardDir, length );
				m_pipeSpawnDataList.push_back( spawnData );
			}
		}

		// For each blocks
		for( XMLElement const *thisBlock = thisSlice->FirstChildElement("Block"); thisBlock != nullptr; thisBlock = thisBlock->NextSiblingElement("Block") )
		{
			std::string blockType = ParseXmlAttribute( *thisBlock, "type", "" );
			
			// Push it in the list
			m_blocksDefinitionList.push_back( blockType );
		}

		sliceIndex++;
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

ePipeForwardDirection TowerDefinition::ParseXmlAttributeForPipeDirection( const XMLElement& element, const char* attributeName, const ePipeForwardDirection &defaultValue )
{
	// Fetch the string
	const char* fetchedString = element.Attribute(attributeName);

	// If not null, see if it contains valid direction
	if( fetchedString != NULL )
	{
		std::string dirStr( fetchedString );

		if( dirStr == "front ")
			return PIPE_DIR_WORLD_FRONT;
		else if( dirStr == "back" )
			return PIPE_DIR_WORLD_BACK;
		else if( dirStr == "right" )
			return PIPE_DIR_WORLD_RIGHT;
		else if( dirStr == "left" )
			return PIPE_DIR_WORLD_LEFT;
		else if( dirStr == "up" )
			return PIPE_DIR_WORLD_UP;
		else if( dirStr == "down" )
			return PIPE_DIR_WORLD_DOWN;
	}

	// If no match found, return default
	return defaultValue;
}