#pragma once
#include "Game/SpriteAndAnimation/IsoSprite.hpp"

enum AnimationPlayStyle
{
	ANIM_PLAYSTYLE_LOOP = 0,
	ANIM_PLAYSTYLE_CLAMP,
	NUM_ANIM_PLAYSTYLES
};

struct AnimationFrame 
{
	IsoSprite*	m_srcIsoSprite		= nullptr;;
	float		m_durationInSeconds	= 0.f;

	AnimationFrame( std::string isoSpriteName, float duration )
	{
		m_srcIsoSprite		= IsoSprite::s_loadedIsoSprites[ isoSpriteName ];
		m_durationInSeconds	= duration;
	}
};

class IsoAnimation
{
private:
	float							m_totalPlayTime = 0.f;
	AnimationPlayStyle				m_playStyle		= ANIM_PLAYSTYLE_LOOP;
	std::string						m_idName		= "";
	std::vector< AnimationFrame* >	m_animFrames;

private:
	 IsoAnimation( XMLElement const &isoanimElement );
	~IsoAnimation();

public:
	static std::map< std::string, IsoAnimation* > s_loadedIsoAnimations;

	static void LoadAllIsoAnimationsFromXML( char const *xmlFileName );
	static void DeleteAllIsoAnimations();
	
	IsoSprite*	Evaluate( float timeElasped ) const;
	IsoSprite*  EvaluateLoopAnimation( float timeElasped ) const;
	IsoSprite*	EvaluateClampAnimation( float timeElasped ) const;

private:
	AnimationPlayStyle GetAnimationPlayStyleFromString( std::string playStyleStr );
};