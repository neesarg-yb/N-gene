#pragma once
#include <map>
#include <string>
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Game/AbilityData.hpp"
#include "Game/SpriteAndAnimation/IsoSprite.hpp"

class ActorDefinition
{
public:
	static void	LoadActorDefinitions( const XMLElement& root );
	static void	DeleteAllDefinitions();

public:
	static std::map< std::string, ActorDefinition* >	s_actorDefinitions;

public:
	std::map< std::string, AbilityData* > m_dataOfAbilities;

	Vector3		m_dimension		= Vector3::ONE_ALL;
	Rgba		m_tintColor		= RGBA_WHITE_COLOR;
	std::string m_animationSet	= "";
	AABB2		m_UV;

	Vector3		m_anchorPoint	= Vector3::ZERO;
	Vector3		m_anchorBoxSize	= Vector3::ZERO;

	float		m_maxHealth		= 0.f;
	int			m_moveRange		= 0;
	int			m_jumpHeight	= 0;
	
	// A map of doable abilities

private:
	 ActorDefinition( const XMLElement& definitionElement );
	~ActorDefinition();
};