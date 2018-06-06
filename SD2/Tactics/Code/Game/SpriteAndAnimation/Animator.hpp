#pragma once
#include "Game/SpriteAndAnimation/AnimationSet.hpp"

class Animator
{
public:
	 Animator( std::string animSetName, std::string defaultAnimName = "idle" );
	~Animator();

private:
	AnimationSet&	m_animationSet;
	IsoAnimation&	m_defaultAnimation;
	IsoAnimation*	m_currentAnimation			= nullptr;
	float			m_timeIntoCurrentAnimation	= 0.f;

public:
	void		Update( float deltaSeconds );
	
	void		Play( std::string animName );
	IsoSprite*	GetCurrentIsoSprite() const;
};