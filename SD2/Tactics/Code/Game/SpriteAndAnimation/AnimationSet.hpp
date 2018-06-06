#pragma once
#include "Game/SpriteAndAnimation/IsoAnimation.hpp"

class Animator;

class AnimationSet
{
	friend Animator;

private:
	std::string								m_idName = "";
	std::map< std::string, IsoAnimation* >	m_aliasNamedIsoAnimations;
	
private:
	 AnimationSet( XMLElement const &animsetElement );
	~AnimationSet();

public:
	static std::map< std::string, AnimationSet* > s_loadedAnimationSets;

	static void LoadAnimationSetFromXML( char const *xmlFileName );
	static void DeleteAllAnimationSets();
};