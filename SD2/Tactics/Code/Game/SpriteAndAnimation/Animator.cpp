#pragma once
#include "Animator.hpp"

Animator::Animator( std::string animSetName, std::string defaultAnimName /* = "idle" */ )
	: m_animationSet( *AnimationSet::s_loadedAnimationSets[ animSetName ] )
	, m_defaultAnimation( *m_animationSet.m_aliasNamedIsoAnimations[ defaultAnimName ] )
{

}

Animator::~Animator()
{

}

void Animator::Update( float deltaSeconds )
{
	m_timeIntoCurrentAnimation += deltaSeconds;
}

void Animator::Play( std::string animName )
{
	m_currentAnimation			= m_animationSet.m_aliasNamedIsoAnimations[ animName ];
	m_timeIntoCurrentAnimation	= 0.f;
}

IsoSprite* Animator::GetCurrentIsoSprite() const
{
	if( m_currentAnimation != nullptr )
		return m_currentAnimation->Evaluate( m_timeIntoCurrentAnimation );
	else
		return m_defaultAnimation.Evaluate( m_timeIntoCurrentAnimation );
}