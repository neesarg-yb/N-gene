#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Tags.hpp"

class EntityDefinition
{
public:
				 EntityDefinition( const XMLElement& definitionElement );
	virtual		~EntityDefinition() {};
	
	// <Entity name="" faction="" tags="">
	std::string m_name			= "Name not initialized";
	Tags		m_startTags;

	// <Size>
	AABB2	m_visualLocalBounds	= AABB2( -0.5f, -0.5f, 0.5f, 0.5f );		// It represents local visual box of an object
	float	m_collisionRadius	= 0.f;										// It represents body collider's boundary

	// <Movement>
	float	m_maxSpeed			= 0.f;
	
	// <SpriteAnimSet>
	SpriteAnimSetDefinition* m_animSetDef = nullptr;

private:
};