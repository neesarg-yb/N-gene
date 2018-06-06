#pragma once
#include "Block.hpp"

Block::Block( Vector3 position, BlockDefinition* definition /* = nullptr */)
	: m_position( position )
	, m_definition( definition )
{
	m_customShader = g_theRenderer->CreateOrGetShaderProgram( "block" );
}

Block::~Block()
{

}

void Block::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Block::Render() const 
{
	if( m_definition != nullptr )	// If not an empty-block
	{

		Vector3 drawPosition = Renderer::GetDrawPositionUsingAnchorPoint( m_position, m_definition->m_anchorBoxSize, m_definition->m_anchorPoint );
		g_theRenderer->UseShaderProgram( m_customShader );
		g_theRenderer->DrawTexturedCube(   drawPosition, m_definition->m_dimension,			m_definition->m_tint_color, 
										   m_definition->m_sprite_texture,	m_definition->m_UV_top, 
										   m_definition->m_UV_side,			m_definition->m_UV_bottom,
										   m_definition->m_outline_texture );
		g_theRenderer->UseShaderProgram( nullptr );
	}
}