#pragma once
#include "Bullet.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Scene.hpp"

Bullet::Bullet( Vector3 const &spawnPosition, Vector3 const &velocity )
	: GameObject( GAME_OBJECT_BULLET )
	, m_velocity( velocity )
{
	// Set Transform
	m_transform = Transform( spawnPosition, Vector3::ZERO, Vector3::ONE_ALL );

	// Set Renderable
	m_renderable		= new Renderable();
	Mesh* sphereMesh	= MeshBuilder::CreateSphere( 0.2f, 5, 5, Vector3::ZERO, RGBA_RED_COLOR );
	m_renderable->SetBaseMesh( sphereMesh );
	Material* sphereMat	= Material::CreateNewFromFile( "Data\\Materials\\default.material" );
	m_renderable->SetBaseMaterial( sphereMat );

	// Parent Transform for Renderable
	m_renderable->m_modelTransform.SetParentAs( &m_transform );
}

Bullet::~Bullet()
{
	delete m_renderable;
}

void Bullet::Update( float deltaSeconds )
{
	// Update the position accrd. to velocity
	Vector3 position = m_transform.GetPosition();
	position		+= m_velocity * deltaSeconds * m_bulletSpeed;
	m_transform.SetPosition( position );
}

void Bullet::AddRenderablesToScene( Scene &activeScene )
{
	activeScene.AddRenderable( *m_renderable );
}

