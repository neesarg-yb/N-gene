#pragma once
#include "Block.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

Block::Block( Vector3 const &position, std::string blockDefinitionName )
	: m_definition( *BlockDefinition::s_definitions[ blockDefinitionName ] )
{
	// PickID if it is a solid block
	if( m_definition.m_isSolid )
		SetPickID( GameObject::GetNewPickID() );

	// Transform
	m_transform.SetPosition( position );

	// Renderable
	Mesh* cubeMesh = MeshBuilder::CreateCube( Vector3::ONE_ALL, Vector3::ZERO, RGBA_WHITE_COLOR, 
												GetUVBoundsFromCoord( m_definition.m_spriteSheetDimension, m_definition.m_uvTopCoord ),			// Top	UV
												GetUVBoundsFromCoord( m_definition.m_spriteSheetDimension, m_definition.m_uvSideCoord ),		// Side UV
												GetUVBoundsFromCoord( m_definition.m_spriteSheetDimension, m_definition.m_uvBottomCoord ) );	// Bot	UV
	m_renderable = new Renderable( cubeMesh, m_definition.m_material );
	m_renderable->SetPickID( GetPickID() );

	// Transform Parenting
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	Vector3 worldPos = m_transform.GetWorldPosition();
}

Block::~Block()
{
	delete m_renderable;
}

void Block::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Block::ObjectSelected()
{
	Vector3 centerPos	= m_transform.GetWorldPosition();
	Vector3 bottomLeft	= centerPos - ( Vector3::ONE_ALL * 0.5f );
	Vector3 topRight	= centerPos + ( Vector3::ONE_ALL * 0.5f );
	DebugRenderWireCube( 0.f, bottomLeft, topRight, RGBA_RED_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
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
