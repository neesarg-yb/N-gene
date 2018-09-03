#pragma once
#include "BlockDefinition.hpp"
#include "Engine/Core/StringUtils.hpp"

using namespace tinyxml2;

BlockDefinitionMap BlockDefinition::s_definitions;

BlockDefinition::BlockDefinition( const XMLElement& root )
{
	// Properties
	m_typeName		= ParseXmlAttribute( root, "name", m_typeName );
	m_isSolid		= ParseXmlAttribute( root, "isSolid", m_isSolid );
	m_isSelectable	= ParseXmlAttribute( root, "isSelectable", m_isSelectable );
	m_isDraggable	= ParseXmlAttribute( root, "isDraggable", m_isDraggable );

	// Material
	std::string matFileName	= ParseXmlAttribute( root, "materialFileName", "" );
	GUARANTEE_OR_DIE( matFileName != "", Stringf( "Error: materialFileName can't be empty for BlockDefinition: %s", m_typeName.c_str() ) );

	XMLDocument matDoc;
	std::string matPath = Stringf( "Data\\Materials\\%s.material", matFileName.c_str() );
	m_material			= Material::CreateNewFromFile( matPath );

	// UVs
	m_spriteSheetDimension	= ParseXmlAttribute( root, "spritesheetSize", m_spriteSheetDimension );
	m_uvTopCoord			= ParseXmlAttribute( root, "topUVCoords", m_uvTopCoord );
	m_uvSideCoord			= ParseXmlAttribute( root, "sideUVCoords", m_uvSideCoord );
	m_uvBottomCoord			= ParseXmlAttribute( root, "bottomUVCoords", m_uvBottomCoord );
}

BlockDefinition::~BlockDefinition()
{
	delete m_material;
}

void BlockDefinition::LoadAllDefinitions( std::string pathToDefinitionXML )
{
	// Get Root
	XMLDocument blockDoc;
	blockDoc.LoadFile( pathToDefinitionXML.c_str() );
	XMLElement* root = blockDoc.RootElement();

	std::string rootName = root->Name();
	GUARANTEE_RECOVERABLE( rootName == "BlockDefinitions", Stringf( "Warning: Root of file \"%s\" is not \"BlockDefinitions\"!", pathToDefinitionXML.c_str() ) );

	// Add all the definitions to pool
	for( XMLElement* thisChild = root->FirstChildElement(); thisChild != nullptr; thisChild = thisChild->NextSiblingElement() )
	{
		// Check if current element is TileDefinition or not
		GUARANTEE_OR_DIE( thisChild->Name() == std::string("BlockDefinition"), Stringf("XML Element named wrong( %s ). It should be named BlockDefinition..!", thisChild->Name() ) );

		std::string	nameOfCurrDef = ParseXmlAttribute( *thisChild, "name", std::string("") );
		BlockDefinitionMap::iterator it = s_definitions.find( nameOfCurrDef );

		// If this definition doesn't exist
		if( nameOfCurrDef != "" && it == s_definitions.end() )
		{
			BlockDefinition* newDefToPush = new BlockDefinition( *thisChild );
			s_definitions[ nameOfCurrDef ] = newDefToPush;
		}
		else
			GUARANTEE_RECOVERABLE( false, Stringf( "BlockDefinition: \"%s\\, already exists in the pool!", nameOfCurrDef.c_str() ) );
	}
}

void BlockDefinition::DeleteAllDefinitions()
{
	// Delete each
	for( BlockDefinitionMap::iterator it = s_definitions.begin(); it != s_definitions.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	// Clear the pool
	s_definitions.clear();
}
