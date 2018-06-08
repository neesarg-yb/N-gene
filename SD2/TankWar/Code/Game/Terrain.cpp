#pragma once
#include "Terrain.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

Terrain::Terrain( Vector3 spawnPosition, uint gridSize, float maxHeight )
	: m_maxHeight( maxHeight )
	, m_sampleSize( gridSize )
{
	// Set transform
	m_transform = Transform( spawnPosition, Vector3::ZERO, Vector3::ONE_ALL );
	m_renderable = new Renderable( m_transform );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Bounds
	Vector2 spawnPositionXZ = Vector2( spawnPosition.x, spawnPosition.z );
	m_terrainBoundsXZ = AABB2( spawnPositionXZ, gridSize * 0.5f, gridSize * 0.5f  );

	// Set Mesh
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );
	mb.AddMeshFromSurfacePatch( [this]( float u, float v ) { return this->SinWavePlane(u,v); }, 
								m_terrainBoundsXZ.mins, m_terrainBoundsXZ.maxs, m_sampleSize, RGBA_GRAY_COLOR );
	mb.End();

	Mesh *terrainMesh = mb.ConstructMesh< Vertex_Lit >();
	m_renderable->SetBaseMesh( terrainMesh );

	// Set Material
	Material *sphereMaterial = Material::CreateNewFromFile( "Data\\Materials\\terrain.material" );
	m_renderable->SetBaseMaterial( sphereMaterial );
}

Terrain::~Terrain()
{
	delete m_renderable;
}

void Terrain::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

float Terrain::GetYCoordinateForMyPositionAt( Vector2 myXZPosition )
{
	//  Logic:
	//  
	//  Hb = lerp( bl_h, br_h, frac_x );
	//  Ht = lerp( tl_h, tr_h, frac.x );
	//  H  = lerp( Hb, Ht, frac.y );

	Vector2 xzFraction;
	xzFraction.x = fmodf( myXZPosition.x, 1.f );
	xzFraction.y = fmodf( myXZPosition.y, 1.f );

	Vector3 bottomLeft	= GiveQuadVertexForMyPositionAt( myXZPosition, TERRAIN_QUAD_BOTTOM_LEFT  );
	Vector3 bottomRight = GiveQuadVertexForMyPositionAt( myXZPosition, TERRAIN_QUAD_BOTTOM_RIGHT );
	Vector3 topLeft		= GiveQuadVertexForMyPositionAt( myXZPosition, TERRAIN_QUAD_TOP_LEFT	 );
	Vector3 topRight	= GiveQuadVertexForMyPositionAt( myXZPosition, TERRAIN_QUAD_TOP_RIGHT	 );

	float	heightBottom	= Interpolate( bottomLeft.y,	bottomRight.y,	xzFraction.x );
	float	heightTop		= Interpolate( topLeft.y,		topRight.y,		xzFraction.x );
	float	myHeight		= Interpolate( heightBottom,	heightTop,		xzFraction.y );

	return myHeight;
}

Vector3 Terrain::SinWavePlane( float u, float v )
{
	Vector3 outPos	= Vector3( u, 0.f, v );
	outPos.y		= sinf( sqrtf( u*u + v*v ) / 10.f ) * m_maxHeight;

	return outPos;
}

Vector3 Terrain::GiveQuadVertexForMyPositionAt( Vector2 myXZPosition, eTerrainQuadVetrex cornerVertex )
{
	// Get coords for Bottom Left Corner
	myXZPosition.x			= floorf( myXZPosition.x );
	myXZPosition.y			= floorf( myXZPosition.y );

	// Get corner cornerStep to add
	Vector2 cornerStep = Vector2::ZERO;
	switch ( cornerVertex )
	{
	case TERRAIN_QUAD_BOTTOM_LEFT:
		cornerStep = Vector2( 0.f, 0.f );
		break;
	case TERRAIN_QUAD_BOTTOM_RIGHT:
		cornerStep = Vector2( 1.f, 0.f );
		break;
	case TERRAIN_QUAD_TOP_LEFT:
		cornerStep = Vector2( 0.f, 1.f );
		break;
	case TERRAIN_QUAD_TOP_RIGHT:
		cornerStep = Vector2( 1.f, 1.f );
		break;
	default:
		GUARANTEE_OR_DIE( false, "Error: Invalid value for enum eTerrainQuadVetrex..!" );
		break;
	}

	// Get uv for that corner
	// m_xzScale 
	Vector2 uvBeforeScale	 = myXZPosition;
			uvBeforeScale	+= cornerStep;

	// Get world position from UVs
	Vector3 cornerPosition	 = SinWavePlane( uvBeforeScale );

//	DebugRenderPoint( 0.f, 1.f, cornerPosition, RGBA_RED_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );

	return cornerPosition;
}