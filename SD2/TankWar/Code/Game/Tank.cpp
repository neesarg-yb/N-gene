#pragma once
#include "Tank.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Material.hpp"

Tank::Tank( Vector3 const &spawnPosition )
{
	// Set transform
	m_transform = Transform( spawnPosition, Vector3::ZERO, Vector3::ONE_ALL );
	m_renderable = new Renderable( m_transform );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	// Set Mesh
	Mesh *sphereMesh = MeshBuilder::CreateSphere( 4.f, 30, 30 );
	m_renderable->SetBaseMesh( sphereMesh );

	// Set Material
	Material *sphereMaterial = Material::CreateNewFromFile( "Data\\Materials\\stone_sphere.material" );
	m_renderable->SetBaseMaterial( sphereMaterial );
}

Tank::~Tank()
{
	delete m_renderable;
}

void Tank::Update( float deltaSeconds )
{
	m_transform.SetPosition( m_transform.GetPosition() + Vector3( 0.f, 0.f, deltaSeconds ) );
}
