#pragma once
#include <string>
#include <map>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Renderer/Texture.hpp"

class BlockDefinition
{
public:
	static void LoadBlockDefinations( const XMLElement& root );
	static void DeleteAllTheDefinitions();

public:
	static std::map< std::string, BlockDefinition* >	s_blockDefinitions;

public:
	const Texture*	m_sprite_texture;
	const Texture*	m_outline_texture	= nullptr;
		  AABB2		m_UV_top;
		  AABB2		m_UV_side;
		  AABB2		m_UV_bottom;
		  Rgba		m_tint_color;

		  Vector3	m_dimension			= Vector3::ONE_ALL;
		  Vector3	m_anchorBoxSize		= Vector3::ZERO;
		  Vector3	m_anchorPoint		= Vector3::ZERO;

private:
	 BlockDefinition( const XMLElement& definitionElement );
	~BlockDefinition();
};