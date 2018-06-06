#pragma once
#include "TileDefinition.hpp"
#include "Engine/Core/XMLUtilities.hpp"

std::map< std::string, TileDefinition* > TileDefinition::s_tileDefinitions;

TileDefinition::TileDefinition( const XMLElement& root )
{
	Vector2 xzDim		= ParseXmlAttribute( root, "dimensions", Vector2::ZERO );
	GUARANTEE_OR_DIE( xzDim != Vector2::ZERO, "TileDefinition: XZ-dimesions can't be Vector2::ZERO ..!" );
	m_dimension			= Vector3( xzDim.x, 0.f, xzDim.y );

	m_tintColor			= ParseXmlAttribute( root, "tintColor", m_tintColor );
	m_yOffsetForDraw	= ParseXmlAttribute( root, "drawOffset", m_yOffsetForDraw );

	m_anchorBoxSize		= ParseXmlAttribute( root, "anchorBoxSize", m_dimension );		// If anchorBoxSize not found, assume that it will be equal to m_dimension
	m_anchorPoint		= ParseXmlAttribute( root, "anchorPoint", m_anchorPoint );
}

TileDefinition::~TileDefinition()
{

}

void TileDefinition::LoadTileDefinations( const XMLElement& root )
{
	std::string rootName = root.Name();
	GUARANTEE_RECOVERABLE( rootName == "TileDefinitions", "Root of Tile.xml should be TileDefinitions..!");

	for( const XMLElement* thisChild = root.FirstChildElement(); thisChild != nullptr; thisChild = thisChild->NextSiblingElement() )
	{
		std::string thisChildName = thisChild->Name();
		GUARANTEE_RECOVERABLE( thisChildName == "TileDefinition", "XML Element named wrong( " + thisChildName + " ). It should be named TileDefinition..!" );

		TileDefinition* defToAdd = new TileDefinition( *thisChild );

		std::string	nameOfCurrDef = ::ParseXmlAttribute( *thisChild, "name", std::string("") );
		std::map< std::string, TileDefinition* >::iterator it = s_tileDefinitions.find( nameOfCurrDef );

		// If definition exists
		if( it != s_tileDefinitions.end() )
		{
			// Delete old definition
			delete it->second;
			it->second = nullptr;

			// Replace it with the new one
			it->second = defToAdd;
		}
		else
			s_tileDefinitions[ nameOfCurrDef ] = defToAdd;
	}
}

void TileDefinition::DeleteAllTheDefinitions()
{
	for( std::map< std::string, TileDefinition* >::iterator it = s_tileDefinitions.begin(); it != s_tileDefinitions.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	s_tileDefinitions.clear();
}