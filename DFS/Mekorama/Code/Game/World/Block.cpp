#pragma once
#include "Block.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

Block::Block( Vector3 const &position, std::string blockDefinitionName )
	: m_definition( *BlockDefinition::s_definitions[ blockDefinitionName ] )
{
	// Transform
	m_transform.SetPosition( position );

	// Renderable
	Mesh* cubeMesh = MeshBuilder::CreateCube( Vector3::ONE_ALL, Vector3::ZERO, RGBA_WHITE_COLOR, 
												GetUVBoundsFromCoord( m_definition.m_spriteSheetDimension, m_definition.m_uvTopCoord ),			// Top	UV
												GetUVBoundsFromCoord( m_definition.m_spriteSheetDimension, m_definition.m_uvSideCoord ),		// Side UV
												GetUVBoundsFromCoord( m_definition.m_spriteSheetDimension, m_definition.m_uvBottomCoord ) );	// Bot	UV
	m_renderable = new Renderable( cubeMesh, m_definition.m_material );

	// Transform Parenting
	m_renderable->m_modelTransform.SetParentAs( &m_transform );
}

Block::~Block()
{
	delete m_renderable;
}

void Block::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

AABB2 Block::GetUVBoundsFromCoord( IntVector2 spriteSheetDimension, IntVector2 uvCoord )
{
	// uvCoord's ( 0, 0 ) is top-left corner
	// Texture's ( 0, 0 ) it bot-left corner
	uvCoord.y = (spriteSheetDimension.y - 1) - uvCoord.y;

	AABB2 uvBounds;

	float xStep		= 1.f / spriteSheetDimension.x;
	float yStep		= 1.f / spriteSheetDimension.y;

	uvBounds.mins	= Vector2( uvCoord.x * xStep, uvCoord.y * yStep );
	uvBounds.maxs	= uvBounds.mins + Vector2( xStep, yStep );

	return uvBounds;
}
