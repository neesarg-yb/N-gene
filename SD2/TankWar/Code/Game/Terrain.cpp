#pragma once
#include "Terrain.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

Vector3 SinWavePlane( float u, float v )
{
	Vector3 outPos	= Vector3( u, 0.f, v );
	outPos.y		= sinf( sqrtf( u*u + v*v ) ) * 10.f;

	return outPos;
}

Terrain::Terrain( Vector3 spawnPosition, Vector2 size )
{
	// Set transform
	m_transform = Transform( spawnPosition, Vector3::ZERO, Vector3::ONE_ALL );
	m_renderable = new Renderable( m_transform );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Bounds
	Vector2 spawnPositionXZ = Vector2( spawnPosition.x, spawnPosition.z );
	m_terrainBoundsXZ = AABB2( spawnPositionXZ, size.x * 0.5f, size.y * 0.5f  );

	// Set Mesh
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );
	mb.AddMeshFromSurfacePatch( SinWavePlane, m_terrainBoundsXZ.mins, m_terrainBoundsXZ.maxs, 10, RGBA_GRAY_COLOR );
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
