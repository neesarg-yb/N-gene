#include "TileDefinition.hpp"

std::map< std::string, TileDefinition* >	TileDefinition::s_definitions;

TileDefinition::TileDefinition()
{

}

TileDefinition::TileDefinition( const XMLElement& root )
{
	std::string rootName = root.Name();
	GUARANTEE_RECOVERABLE( rootName == "TileDefinitions", "Root element of Tile.xml should to be TileDefinitions..!" );

	for( XMLElement* thisChild = (XMLElement*) root.FirstChildElement(); thisChild != nullptr; thisChild = thisChild->NextSiblingElement() )
	{
		// Check if current element is TileDefinition or not
		GUARANTEE_RECOVERABLE( std::string( thisChild->Name() ) == "TileDefinition", "XML Element named wrong( " + std::string( thisChild->Name() ) + " ). It should be named TileDefinition..!" );

		std::string	nameOfCurrDef = ParseXmlAttribute( *thisChild, "name", std::string("") );
		std::map< std::string, TileDefinition* >::iterator it = s_definitions.find( nameOfCurrDef );

		// If this definition doesn't exist
		if( nameOfCurrDef != "" && it == s_definitions.end() )
		{
			// Create new
			TileDefinition* newDef	= new TileDefinition();		
			newDef->m_tileName		= nameOfCurrDef;
			newDef->m_startTags		= ParseXmlAttribute( *thisChild, "tags", m_startTags );
			
			IntVector2 spriteCoords				= ParseXmlAttribute( *thisChild, "baseSpriteCoords", spriteCoords );
			newDef->m_baseTileUVTextBounds		= g_tileSpriteSheet->GetTexCoordsForSpriteCoords( spriteCoords );
			newDef->m_baseTileSpriteTint		= ParseXmlAttribute( *thisChild, "baseSpriteTint", m_baseTileSpriteTint );
			
			if( thisChild->Attribute( "overlaySpriteCoords" ) != NULL )
				newDef->m_overlayEnabled = true;

			spriteCoords						= ParseXmlAttribute( *thisChild, "overlaySpriteCoords", spriteCoords );
			newDef->m_overlayTileUVTextBounds	= g_tileSpriteSheet->GetTexCoordsForSpriteCoords( spriteCoords );
			newDef->m_overlayTileSpriteTint		= ParseXmlAttribute( *thisChild, "overlaySpriteTint", m_overlayTileSpriteTint );

			newDef->m_allowsSight				= ParseXmlAttribute( *thisChild, "allowsSight", m_allowsSight );
			newDef->m_allowsWalking				= ParseXmlAttribute( *thisChild, "allowsWalking", m_allowsWalking );
			newDef->m_allowsFlying				= ParseXmlAttribute( *thisChild, "allowsFlying", m_allowsFlying );
			newDef->m_allowsSwimming			= ParseXmlAttribute( *thisChild, "allowsSwimming", m_allowsSwimming );
			newDef->m_texelValue				= ParseXmlAttribute( *thisChild, "texelValue", m_texelValue );

			// Add it to static vector..
			s_definitions[ newDef->m_tileName ] = newDef;
		}
	}
}

void TileDefinition::LoadTileDefinitions( const XMLElement& root )
{
	new TileDefinition( root );
}

TileDefinition* TileDefinition::GetTileDefinitionForRGB( const Rgba& texel )
{
	TileDefinition* toReturn = nullptr;
	Rgba texal_a255 = Rgba( texel.r, texel.g, texel.b, 255 );

	for( std::map< std::string, TileDefinition*>::iterator it = s_definitions.begin(); it != s_definitions.end(); it++ )
	{
		TileDefinition* thisDef = it->second;
		if( thisDef->m_texelValue == texal_a255 ) {
			toReturn = thisDef;
			break;
		}
	}

	return toReturn;
}

TileDefinition* ParseXmlAttribute( const XMLElement& element, const char* attributeName, TileDefinition* defaultValue )
{
	// Get tile definition name from element
	std::string tileDefName = ParseXmlAttribute( element, attributeName, std::string("") );

	// If this definition exist, return a pointer to it
	std::map< std::string, TileDefinition* >::iterator it = TileDefinition::s_definitions.find( tileDefName );

	// If TileDefination not found, DIE
	GUARANTEE_OR_DIE( it != TileDefinition::s_definitions.end(), "TileDefination by name " + tileDefName + " not found..!" );

	if( tileDefName != "" && it != TileDefinition::s_definitions.end() )
	{
		return it->second;
	}

	return defaultValue;
}