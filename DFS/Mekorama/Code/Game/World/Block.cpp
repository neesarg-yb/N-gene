#pragma once
#include "Block.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

Block::Block( Vector3 const &position, std::string blockDefinitionName )
	: m_definition( BlockDefinition::s_definitions[ blockDefinitionName ] )
{
	// PickID if it is a selectable block
	if( m_definition->m_isSelectable )
		SetPickID( GameObject::GetNewPickID() );

	// Transform
	m_transform.SetPosition( position );

	// Renderable
	m_renderable = CreateNewRenderable();
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

void Block::ChangeBlockTypeTo( std::string definitionName )
{
	if( m_definition->m_typeName == definitionName )
		return;

	// Change block definition
	m_definition = BlockDefinition::s_definitions[ definitionName ];

	// Reset the Renderable
	delete m_renderable;
	m_renderable = nullptr;
	m_renderable = CreateNewRenderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );
}

Renderable* Block::CreateNewRenderable()
{
	Mesh* renderableMesh = nullptr;

	// Stairs has a different shape
	if( m_definition->m_typeName == "Stairs" )
	{
		Vector3 firstStepSize		= Vector3( 1.00f,  0.33f, 1.00f );
		Vector3 secondStepSize		= Vector3( 0.67f,  0.33f, 0.67f );
		Vector3	thirdStepSize		= Vector3( 0.33f,  0.33f, 0.33f );

		Vector3 firstStepCenter		= Vector3( 0.00f, -0.33f, 0.00f );
		Vector3 secondStepCenter	= Vector3( 0.00f,  0.00f, 0.00f );
		Vector3 thirdStepCenter		= Vector3( 0.00f,  0.33f, 0.00f );

		// Build a Stair which has 3 steps..
		MeshBuilder mbStairs;
		mbStairs.Begin( PRIMITIVE_TRIANGES, true );
		mbStairs.AddCube(	firstStepSize, firstStepCenter, RGBA_WHITE_COLOR,
							GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvTopCoord ),
							GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvSideCoord ),
							GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvBottomCoord ) );
		mbStairs.AddCube(	secondStepSize, secondStepCenter, RGBA_WHITE_COLOR,
							GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvTopCoord ),
							GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvSideCoord ),
							GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvBottomCoord ) );
		mbStairs.AddCube(	thirdStepSize, thirdStepCenter, RGBA_WHITE_COLOR,
							GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvTopCoord ),
							GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvSideCoord ),
							GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvBottomCoord ) );
		mbStairs.End();

		renderableMesh = mbStairs.ConstructMesh<Vertex_Lit>();

	}
	else // All other blocks have the same mesh
	{
		renderableMesh = MeshBuilder::CreateCube(	Vector3::ONE_ALL, Vector3::ZERO, RGBA_WHITE_COLOR, 
													GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvTopCoord ),		// Top	UV
													GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvSideCoord ),		// Side UV
													GetUVBoundsFromCoord( m_definition->m_spriteSheetDimension, m_definition->m_uvBottomCoord ) );	// Bot	UV
	}
	
	Material	*dupMaterial	= new Material( *m_definition->m_material );
	Renderable	*renderable		= new Renderable( renderableMesh, dupMaterial );
	renderable->SetPickID( GetPickID() );

	return renderable;
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
