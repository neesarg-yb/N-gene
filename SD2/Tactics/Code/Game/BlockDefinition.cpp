#pragma once
#include "BlockDefinition.hpp"
#include "Game/GameCommon.hpp"

std::map< std::string, BlockDefinition* > BlockDefinition::s_blockDefinitions;

BlockDefinition::BlockDefinition( const XMLElement& definitionElement )
{
	// Get sprite texture
	std::string spriteName			= ParseXmlAttribute( definitionElement, "spriteTextureName", std::string("") );
	std::string spriteTexturePath	= "Data\\Images\\" + spriteName;
	GUARANTEE_OR_DIE( spriteName != "", "BlockDefinition: spriteTextureName can't be empty string, in XML!" );

	m_sprite_texture				= g_theRenderer->CreateOrGetTexture( spriteTexturePath );
	IntVector2 spriteSheetDimension = ParseXmlAttribute( definitionElement, "spriteSheetDimension", IntVector2::ZERO );
	GUARANTEE_OR_DIE( spriteSheetDimension != IntVector2::ZERO, "BlockDefinition: spriteSheetDimension can't be IntVector2::ZERO ..!" );
	
	// To set UVs
	SpriteSheet	tempSpriteSheet		= SpriteSheet( *m_sprite_texture, spriteSheetDimension.x, spriteSheetDimension.y );
	IntVector2	topTexCoords		= ParseXmlAttribute( definitionElement, "texCoordsForTop",		spriteSheetDimension );
	IntVector2	sideTexCoords		= ParseXmlAttribute( definitionElement, "texCoordsForSide",		spriteSheetDimension );
	IntVector2	bottomTexCoords		= ParseXmlAttribute( definitionElement, "texCoordsForBottom",	spriteSheetDimension );
	GUARANTEE_OR_DIE( topTexCoords		!= spriteSheetDimension, "BlockDefinition: texCoordsForTop couldn't set up properly..!" );
	GUARANTEE_OR_DIE( sideTexCoords		!= spriteSheetDimension, "BlockDefinition: texCoordsForSide couldn't set up properly..!" );
	GUARANTEE_OR_DIE( bottomTexCoords	!= spriteSheetDimension, "BlockDefinition: texCoordsForBottom couldn't set up properly..!" );

	m_UV_top	= tempSpriteSheet.GetTexCoordsForSpriteCoords( topTexCoords );
	m_UV_side	= tempSpriteSheet.GetTexCoordsForSpriteCoords( sideTexCoords );
	m_UV_bottom	= tempSpriteSheet.GetTexCoordsForSpriteCoords( bottomTexCoords );

	m_tint_color	=	ParseXmlAttribute( definitionElement, "tintColor", RGBA_WHITE_COLOR );

	// Get outline texture
	spriteName			= ParseXmlAttribute( definitionElement, "boxOutlineTextureName", std::string("") );
	spriteTexturePath	= "Data\\Images\\" + spriteName;
	if( spriteName == "" )
		m_outline_texture = nullptr;
	else
	{
		m_outline_texture = g_theRenderer->CreateOrGetTexture( spriteTexturePath );
	}

	// Get dimensions and Anchor point
	m_dimension			= ParseXmlAttribute( definitionElement, "dimensions", m_dimension );
	m_anchorBoxSize		= ParseXmlAttribute( definitionElement, "anchorBoxSize", m_dimension );		// If anchorBoxSize not found, assume that it will be equal to m_dimension
	m_anchorPoint		= ParseXmlAttribute( definitionElement, "anchorPoint", m_anchorPoint );
}

BlockDefinition::~BlockDefinition()
{

}

void BlockDefinition::LoadBlockDefinations( const XMLElement& root )
{
	std::string rootName = root.Name();
	GUARANTEE_RECOVERABLE( rootName == "BlockDefinitions", "Root of Blocks.xml should be BlockDefinitions..!");

	for( const XMLElement* thisChild = root.FirstChildElement(); thisChild != nullptr; thisChild = thisChild->NextSiblingElement() )
	{
		std::string thisChildName = thisChild->Name();
		GUARANTEE_RECOVERABLE( thisChildName == "BlockDefinition", "XML Element named wrong( " + thisChildName + " ). It should be named BlockDefinition..!" );
		
		BlockDefinition* defToAdd = new BlockDefinition( *thisChild );

		std::string	nameOfCurrDef = ::ParseXmlAttribute( *thisChild, "name", std::string("") );
		std::map< std::string, BlockDefinition* >::iterator it = s_blockDefinitions.find( nameOfCurrDef );

		// If definition exists
		if( it != s_blockDefinitions.end() )
		{
			// Delete old definition
			delete it->second;
			it->second = nullptr;

			// Replace it with the new one
			it->second = defToAdd;
		}
		else
			s_blockDefinitions[ nameOfCurrDef ] = defToAdd;
	}
}

void BlockDefinition::DeleteAllTheDefinitions()
{
	for( std::map< std::string, BlockDefinition* >::iterator it = s_blockDefinitions.begin(); it != s_blockDefinitions.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	s_blockDefinitions.clear();
}