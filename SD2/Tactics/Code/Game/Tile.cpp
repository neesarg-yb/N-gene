#pragma once
#include "Tile.hpp"

Tile::Tile( const Vector3 worldPosition, TileDefinition* definition )
	: m_position( worldPosition )
	, m_definition( definition )
{

}

Tile::~Tile()
{

}

void Tile::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Tile::Render() const 
{
	Vector3 correctedPosition = m_position - Vector3( 0.f, 0.49f + m_definition->m_yOffsetForDraw, 0.f );
	Vector3 drawPosition	  = Renderer::GetDrawPositionUsingAnchorPoint( correctedPosition, m_definition->m_anchorBoxSize, m_definition->m_anchorPoint );
	g_theRenderer->DrawTexturedCube( drawPosition, m_definition->m_dimension, m_definition->m_tintColor );
}

void Tile::RenderInNewTint( const Rgba newTint) const
{
	Vector3 correctedPosition = m_position - Vector3( 0.f, 0.49f + m_definition->m_yOffsetForDraw, 0.f );
	Vector3 drawPosition	  = Renderer::GetDrawPositionUsingAnchorPoint( correctedPosition, m_definition->m_anchorBoxSize, m_definition->m_anchorPoint );
	g_theRenderer->DrawTexturedCube( drawPosition, m_definition->m_dimension, newTint );
}