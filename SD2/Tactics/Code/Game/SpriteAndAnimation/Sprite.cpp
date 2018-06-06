#pragma once
#include "Sprite.hpp"
#include "Engine/Core/XMLUtilities.hpp"

using namespace tinyxml2;

std::map< std::string, Sprite* >	Sprite::s_loadedSprites;

Sprite::Sprite( const XMLElement& spriteElement )
{
	m_idName = ParseXmlAttribute( spriteElement, "id", "" );
	GUARANTEE_RECOVERABLE( std::string(m_idName) != "", "WARNING: id of sprite xml-element not found!" );

	// Diffuse Texture Element
	XMLElement const *diffuseTextureElement = spriteElement.FirstChildElement( "diffuse" );
	char const * srcPath = ParseXmlAttribute( *diffuseTextureElement, "src", "" );
	GUARANTEE_OR_DIE( std::string(srcPath) != "", "WARNING: srcPath is empty!" );
	m_texture = g_theRenderer->CreateOrGetTexture( srcPath );

	// PPU Element
	XMLElement const *ppuElement = spriteElement.FirstChildElement( "ppu" );
	m_PPU = ParseXmlAttribute( *ppuElement, "count", m_PPU );

	// UV Element
	XMLElement const *uvElement = spriteElement.FirstChildElement( "uv" );
	m_UVsInPixels = ParseXmlAttribute( *uvElement, "uvs", m_UVsInPixels );			// ( 0, 0 ) is at Top-Left corner
	float textureHeight	= (float) m_texture->GetHeight();
	float spriteHeight	= m_UVsInPixels.maxs.y - m_UVsInPixels.mins.y;
	AABB2 correctedBounds;
	correctedBounds.mins	= Vector2( m_UVsInPixels.mins.x, textureHeight - m_UVsInPixels.mins.y - spriteHeight );
	correctedBounds.maxs	= Vector2( m_UVsInPixels.maxs.x, textureHeight - m_UVsInPixels.maxs.y + spriteHeight );
	m_UVsInPixels			= correctedBounds;
	
	// Pivot Element
	XMLElement const *pivotElement = spriteElement.FirstChildElement( "pivot" );
	m_pivotPoint = ParseXmlAttribute( *pivotElement, "xy", m_pivotPoint );

	// Calculate worldDimension
	AABB2 UVBoundInWorldUnits	= m_UVsInPixels / m_PPU;
	float width					= UVBoundInWorldUnits.maxs.x - UVBoundInWorldUnits.mins.x;
	float height				= UVBoundInWorldUnits.maxs.y - UVBoundInWorldUnits.mins.y;
	m_worldDimension			= Vector2( width, height );

	// Calculate Normalized UVs
	int textureWidth			= m_texture->GetWidth();
	m_UVsNormalized.mins		= Vector2( m_UVsInPixels.mins.x / textureWidth, m_UVsInPixels.mins.y / textureHeight );
	m_UVsNormalized.maxs		= Vector2( m_UVsInPixels.maxs.x / textureWidth, m_UVsInPixels.maxs.y / textureHeight );
}

Sprite::~Sprite()
{

}

void Sprite::LoadSpritesFromXML( const char* xmlFileName )
{
	// Loading: XMLDocument
	XMLDocument spriteXMLDoc;
	spriteXMLDoc.LoadFile( xmlFileName );
	XMLElement const *spriteDefRoot = spriteXMLDoc.RootElement();
	
	char const *rootName = spriteDefRoot->Name();
	GUARANTEE_RECOVERABLE( std::string(rootName) == "sprites", "Error: sprite xml's rootName should be sprites!" );

	for( const XMLElement* thisChild = spriteDefRoot->FirstChildElement( "sprite" ); thisChild != nullptr; thisChild = thisChild->NextSiblingElement( "sprite" ) )
	{
		// New Sprite to add
		Sprite* spriteToAdd = new Sprite( *thisChild );

		char const *idOfCurrSprite = ::ParseXmlAttribute( *thisChild, "id", "" );
		std::map< std::string, Sprite* >::iterator it = s_loadedSprites.find( idOfCurrSprite );

		// If sprite exists
		if( it != s_loadedSprites.end() )
		{
			// Delete old sprite
			delete it->second;
			it->second = nullptr;

			// Replace it with the new one
			it->second = spriteToAdd;
		}
		else
			s_loadedSprites[ idOfCurrSprite ] = spriteToAdd;
	}
}

void Sprite::DeleteAllSprites()
{
	for( std::map< std::string, Sprite* >::iterator it = s_loadedSprites.begin(); it != s_loadedSprites.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	s_loadedSprites.clear();
}

void Sprite::Render( Matrix44 const &transformMatrix ) const
{
	TODO("Start using m_pivitPoint");
	AABB2 localBounds = AABB2( m_worldDimension * -0.5f, m_worldDimension * 0.5f );

	g_theRenderer->DrawTexturedAABB( transformMatrix, localBounds, *m_texture, m_UVsNormalized.mins, m_UVsNormalized.maxs, RGBA_WHITE_COLOR );
}