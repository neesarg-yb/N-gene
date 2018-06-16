#include "Turret.hpp"
#include "Game/Tank.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

Turret::Turret( Tank &parentTank )
	: m_parentTank( parentTank )
{
	// Head Sphere
	m_headTransform.SetParentAs( &m_parentTank.m_transform );
	m_headTransform.SetPosition( Vector3( 0.f, m_parentTank.m_height, 0.f ) );
	// Renderable
	m_barrelRenderable	= new Renderable();
	Mesh* sphereMesh	= MeshBuilder::CreateSphere( m_parentTank.m_height * 0.5f, 10, 10, Vector3( 0.f, m_parentTank.m_height, 0.f ) );
	Material *material	= Material::CreateNewFromFile( "Data\\Materials\\default.material" );
	m_barrelRenderable->SetBaseMesh( sphereMesh );
	m_barrelRenderable->SetBaseMaterial( material );
	m_barrelRenderable->m_modelTransform.SetParentAs( &m_headTransform );
}

Turret::~Turret()
{
	delete m_barrelRenderable;
}