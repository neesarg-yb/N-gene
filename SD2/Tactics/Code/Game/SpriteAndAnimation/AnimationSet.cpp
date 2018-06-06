#pragma once
#include "Engine/Core/XMLUtilities.hpp"
#include "Game/SpriteAndAnimation/AnimationSet.hpp"

using namespace tinyxml2;

std::map< std::string, AnimationSet* > AnimationSet::s_loadedAnimationSets;

AnimationSet::AnimationSet( XMLElement const &animsetElement )
{
	// Set m_idName
	m_idName = ParseXmlAttribute( animsetElement, "id", "" );
	GUARANTEE_RECOVERABLE( std::string(m_idName) != "", "Error: animset XMLElement's id not found!" );

	// Set m_aliasNamedIsoAnimations
	for( XMLElement const * thisAliasElement  = animsetElement.FirstChildElement( "set" ); 
							thisAliasElement != nullptr;
							thisAliasElement  = thisAliasElement->NextSiblingElement("set") )
	{
		std::string aliasAnimName			= ParseXmlAttribute( *thisAliasElement, "id", "" );
		XMLElement const * childAnimElement = thisAliasElement->FirstChildElement();
		std::string isoAnimationIdName		= ParseXmlAttribute( *childAnimElement, "id", "" );
		GUARANTEE_RECOVERABLE( std::string(aliasAnimName)		!= "", "Error: animset->set XMLElement's id not found!" );
		GUARANTEE_RECOVERABLE( std::string(isoAnimationIdName)	!= "", "Error: animset->set->anim XMLElement's id not found!" );

		m_aliasNamedIsoAnimations[ aliasAnimName ] = IsoAnimation::s_loadedIsoAnimations[ isoAnimationIdName ];
	}
}

AnimationSet::~AnimationSet()
{

}

void AnimationSet::LoadAnimationSetFromXML( char const *xmlFileName )
{
	// Loading: XMLDocument
	XMLDocument animSetXMLDoc;
	animSetXMLDoc.LoadFile( xmlFileName );
	XMLElement const *animSetRoot = animSetXMLDoc.RootElement();

	char const *rootName = animSetRoot->Name();
	GUARANTEE_RECOVERABLE( std::string(rootName) == "animset", "Error: animset xml's rootName should be animset!" );
	
	// New AnimSet to add
	AnimationSet* animSetToAdd = new AnimationSet( *animSetRoot );

	char const *idOfCurrAnimSet = ::ParseXmlAttribute( *animSetRoot, "id", "" );
	std::map< std::string, AnimationSet* >::iterator it = s_loadedAnimationSets.find( idOfCurrAnimSet );

	// If AnimSet exists
	if( it != s_loadedAnimationSets.end() )
	{
		// Delete old AnimSet
		delete it->second;
		it->second = nullptr;

		// Replace it with the new one
		it->second = animSetToAdd;
	}
	else
		s_loadedAnimationSets[ idOfCurrAnimSet ] = animSetToAdd;
}

void AnimationSet::DeleteAllAnimationSets()
{
	for( std::map< std::string, AnimationSet* >::iterator it = s_loadedAnimationSets.begin(); it != s_loadedAnimationSets.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	s_loadedAnimationSets.clear();
}