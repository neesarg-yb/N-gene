#pragma once
#include "IsoAnimation.hpp"
#include "Engine/Core/XMLUtilities.hpp"

using namespace tinyxml2;

std::map< std::string, IsoAnimation* > IsoAnimation::s_loadedIsoAnimations;

IsoAnimation::IsoAnimation( XMLElement const &isoanimElement )
{
	// Setup m_idName
	m_idName = ParseXmlAttribute( isoanimElement, "id", "" );
	GUARANTEE_RECOVERABLE( std::string(m_idName) != "", "Error: isoanim XMLElement's id not found!" );

	// Setup m_playStyle
	std::string loopStyle	= ParseXmlAttribute( isoanimElement, "loop", "loop" );
	m_playStyle				= GetAnimationPlayStyleFromString( loopStyle );

	// Setup all the AnimationFrame(s)
	for( XMLElement const * thisFrameElement  = isoanimElement.FirstChildElement( "frame" ); 
							thisFrameElement != nullptr;
							thisFrameElement  = thisFrameElement->NextSiblingElement("frame") )
	{
		std::string srcIsoSpriteName	 = ParseXmlAttribute( *thisFrameElement, "src", "" );
		float		duration			 = ParseXmlAttribute( *thisFrameElement, "duration", 0.f );

		// Cumulatively add duration to get m_totalPlayTime
		m_totalPlayTime					+= duration;
		
		AnimationFrame* newFrame		 = new AnimationFrame( srcIsoSpriteName, duration );
		m_animFrames.push_back( newFrame );
	}

	GUARANTEE_RECOVERABLE( m_totalPlayTime != 0.f, "WARNING: IsoAnimation " + m_idName + "'s totalPlayTime == 0. This can cause the infinite loop!" );
}

IsoAnimation::~IsoAnimation()
{
	while ( m_animFrames.size() > 0 )
	{
		delete m_animFrames[0];
		m_animFrames[0] = nullptr;

		m_animFrames.erase( m_animFrames.begin() );
	}
}

void IsoAnimation::LoadAllIsoAnimationsFromXML( char const * xmlFileName )
{
	// Loading: XMLDocument
	XMLDocument isoAnimXMLDoc;
	isoAnimXMLDoc.LoadFile( xmlFileName );
	XMLElement const *isoAnimRoot = isoAnimXMLDoc.RootElement();

	char const *rootName = isoAnimRoot->Name();
	GUARANTEE_RECOVERABLE( std::string(rootName) == "isoanims", "Error: isoanim xml's rootName should be isoanims!" );

	for( const XMLElement* thisChild = isoAnimRoot->FirstChildElement( "isoanim" ); thisChild != nullptr; thisChild = thisChild->NextSiblingElement( "isoanim" ) )
	{
		// New IsoAnim to add
		IsoAnimation* isoAnimToAdd = new IsoAnimation( *thisChild );

		char const *idOfCurrIsoAnim = ::ParseXmlAttribute( *thisChild, "id", "" );
		std::map< std::string, IsoAnimation* >::iterator it = s_loadedIsoAnimations.find( idOfCurrIsoAnim );

		// If IsoAnim exists
		if( it != s_loadedIsoAnimations.end() )
		{
			// Delete old IsoAnim
			delete it->second;
			it->second = nullptr;

			// Replace it with the new one
			it->second = isoAnimToAdd;
		}
		else
			s_loadedIsoAnimations[ idOfCurrIsoAnim ] = isoAnimToAdd;
	}
}

void IsoAnimation::DeleteAllIsoAnimations()
{
	for( std::map< std::string, IsoAnimation* >::iterator it = s_loadedIsoAnimations.begin(); it != s_loadedIsoAnimations.end(); it++ )
	{
		delete it->second;
		it->second = nullptr;
	}

	s_loadedIsoAnimations.clear();
}

IsoSprite* IsoAnimation::Evaluate( float timeElasped ) const
{
	switch (m_playStyle)
	{
	case ANIM_PLAYSTYLE_LOOP:
		return EvaluateLoopAnimation ( timeElasped );
		break;

	case ANIM_PLAYSTYLE_CLAMP:
		return EvaluateClampAnimation( timeElasped );
		break;

	default:
		return EvaluateLoopAnimation ( timeElasped );
		break;
	}
}

AnimationPlayStyle IsoAnimation::GetAnimationPlayStyleFromString( std::string playStyleStr )
{
	if( playStyleStr == "loop" )
		return ANIM_PLAYSTYLE_LOOP;
	if( playStyleStr == "clamp" )
		return ANIM_PLAYSTYLE_CLAMP;

	return ANIM_PLAYSTYLE_LOOP;
}

IsoSprite* IsoAnimation::EvaluateLoopAnimation( float timeElasped ) const
{
	while ( timeElasped > m_totalPlayTime )
		timeElasped -= m_totalPlayTime;

	// Go to the current AnimationFrame
	for( unsigned int i = 0; i < m_animFrames.size(); i++ )
	{
		timeElasped -= m_animFrames[i]->m_durationInSeconds;
		if( timeElasped <= 0 )
			return m_animFrames[i]->m_srcIsoSprite;
	}

	// Flow should never reach here..
	return m_animFrames.back()->m_srcIsoSprite;
}

IsoSprite* IsoAnimation::EvaluateClampAnimation( float timeElasped ) const
{
	// Go to the current AnimationFrame
	for( unsigned int i = 0; i < m_animFrames.size(); i++ )
	{
		timeElasped -= m_animFrames[i]->m_durationInSeconds;
		if( timeElasped <= 0 )
			return m_animFrames[i]->m_srcIsoSprite;
	}

	return m_animFrames.back()->m_srcIsoSprite;
}