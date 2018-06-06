#pragma once
#include "IsoSprite.hpp"
#include "Engine/Core/XMLUtilities.hpp"

using namespace tinyxml2;

std::map< std::string, IsoSprite* > IsoSprite::s_loadedIsoSprites;

IsoSprite::IsoSprite( XMLElement const &isoSpriteElement )
{
	// Set m_idName
	m_idName = ParseXmlAttribute( isoSpriteElement, "id", "" );
	GUARANTEE_RECOVERABLE( std::string(m_idName) != "", "Error: isosprite XMLElement's id not found!" );

	// Set all four SpritesWithScale
	//	Note: I'm assuming that the XML Data will be defined according to the order of FacingDirection ENUM
	XMLElement const * thisFacingElement = isoSpriteElement.FirstChildElement( "facing" );
	for( unsigned int faceDir = 0; faceDir < NUM_FACING_DIRECTIONS; faceDir++ )
	{
		GUARANTEE_OR_DIE( thisFacingElement != nullptr, std::string("Error: Not all facings of XMLElemts found") );

		Vector2		faceConfig	= ParseXmlAttribute( *thisFacingElement,	"dir",		Vector2::ONE_ONE );
		char const *srcIdName	= ParseXmlAttribute( *thisFacingElement,	"src",		"" );
		Vector2		scale		= ParseXmlAttribute( *thisFacingElement,	"scale",	Vector2::ONE_ONE );

		m_faces[ faceDir ] = new SpriteWithScale( srcIdName, scale );
		
		thisFacingElement = thisFacingElement->NextSiblingElement( "facing" ); 
	}
}

IsoSprite::~IsoSprite()
{
	for( unsigned int faceDir = 0; faceDir < NUM_FACING_DIRECTIONS; faceDir++ )
	{
		delete m_faces[ faceDir ];
		m_faces[ faceDir ] = nullptr;
	}
}

void IsoSprite::LoadAllIsoSpritesFromXML( char const * xmlFileName )
{
	// Loading: XMLDocument
	XMLDocument isoSpriteXMLDoc;
	isoSpriteXMLDoc.LoadFile( xmlFileName );
	XMLElement const *isoSpriteDefRoot = isoSpriteXMLDoc.RootElement();

	char const *rootName = isoSpriteDefRoot->Name();
	GUARANTEE_RECOVERABLE( std::string(rootName) == "isosprites", "Error: isosprite xml's rootName should be isosprites!" );

	for( const XMLElement* thisChild = isoSpriteDefRoot->FirstChildElement( "isosprite" ); thisChild != nullptr; thisChild = thisChild->NextSiblingElement( "isosprite" ) )
	{
		// New IsoSprite to add
		IsoSprite* isoSpriteToAdd = new IsoSprite( *thisChild );

		char const *idOfCurrIsoSprite = ::ParseXmlAttribute( *thisChild, "id", "" );
		std::map< std::string, IsoSprite* >::iterator it = s_loadedIsoSprites.find( idOfCurrIsoSprite );

		// If sprite exists
		if( it != s_loadedIsoSprites.end() )
		{
			// Delete old sprite
			delete it->second;
			it->second = nullptr;

			// Replace it with the new one
			it->second = isoSpriteToAdd;
		}
		else
			s_loadedIsoSprites[ idOfCurrIsoSprite ] = isoSpriteToAdd;
	}
}

void IsoSprite::DeleteAllIsoSprites()
{
	for( std::map< std::string, IsoSprite* >::iterator it = s_loadedIsoSprites.begin(); it != s_loadedIsoSprites.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	s_loadedIsoSprites.clear();
}

SpriteWithScale* IsoSprite::GetSpriteWithScaleForActorLookingInDirection( Vector3 const &actorForward )
{
	Matrix44 cameraViewMatrix			= g_theRenderer->s_current_camera->m_view_matrix;
	Vector3  actorForwardInCameraSpace	= cameraViewMatrix.Multiply( actorForward, 0.f );

	// We care about just XZ-Plane
	Vector2 actorForwardXZ( actorForwardInCameraSpace.x, actorForwardInCameraSpace.z );
	
	if( actorForwardXZ.x > 0 )		// X-Plane Right
	{
		if( actorForwardXZ.y > 0 )	// Z-Plane
			return m_faces[ AWAY_RIGHT ];
		else
			return m_faces[ TOWARD_RIGHT ];
	}
	else							// X-Plane Left
	{
		if( actorForwardXZ.y > 0 )	// Z-Plane
			return m_faces[ AWAY_LEFT ];
		else
			return m_faces[ TOWARD_LEFT ];
	}
}