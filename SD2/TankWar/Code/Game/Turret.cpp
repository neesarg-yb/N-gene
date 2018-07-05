#pragma once
#include "Turret.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/Tank.hpp"
#include "Game/Bullet.hpp"

#define TANK_HALF_LENGTH 0.5f

Turret::Turret( Tank &parentTank )
	: m_parentTank( parentTank )
{
	// Head Sphere
	m_headTransform.SetParentAs( &m_parentTank.m_transform );
	m_headTransform.SetPosition( Vector3( 0.f, m_parentTank.m_height * 2.f, TANK_HALF_LENGTH ) );		// m_parentTank.m_height * "2.f" because Renderable of Tank has Y-Offset equal to m_height, already
	// Renderable
	m_barrelRenderable	= new Renderable();
	MeshBuilder turretMB;
	turretMB.Begin( PRIMITIVE_TRIANGES, true );
	turretMB.AddSphere( m_parentTank.m_height * 0.5f, 10, 10, Vector3::ZERO );
	float turretLength = 2.f;
	turretMB.AddCube(	Vector3( 0.3f, 0.3f, turretLength ), 
						Vector3( 0.0f, 0.0f, turretLength * 0.5f ), 
						RGBA_RED_COLOR );
	turretMB.End();

	Mesh* sphereMesh	= turretMB.ConstructMesh<Vertex_Lit>();
	Material *material	= Material::CreateNewFromFile( "Data\\Materials\\default.material" );
	m_barrelRenderable->SetBaseMesh( sphereMesh );
	m_barrelRenderable->SetBaseMaterial( material );
	m_barrelRenderable->m_modelTransform.SetParentAs( &m_headTransform );
}

Turret::~Turret()
{
	delete m_barrelRenderable;
}

void Turret::LookAtPosition( Vector3 targetPosInWorldSpace, float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	UNUSED( deltaSeconds );

	// Get Look At in World Space
	Vector3		turretUpInWorld		= m_headTransform.GetWorldTransformMatrix().GetJColumn();
	Matrix44	worldLookAtMat		= Matrix44::MakeLookAtView( targetPosInWorldSpace, m_headTransform.GetWorldPosition(), turretUpInWorld );

	// Get World to Tank Matrix
	Matrix44	worldToTankMat;
	Matrix44	tankToWorldMat		= m_parentTank.m_transform.GetWorldTransformMatrix();
	bool		invSuccess			= tankToWorldMat.GetInverse( worldToTankMat );
	GUARANTEE_RECOVERABLE( invSuccess, "Error: Couldn't inverse the tankToWorldMatrix!" );

	// Convert Look At to Tank's Space, now
	Matrix44	turrateBasesInTankSpace;
	turrateBasesInTankSpace.Append( worldToTankMat );
	turrateBasesInTankSpace.Append( worldLookAtMat );
	// Ignore translation if there are any
	turrateBasesInTankSpace.SetTColumn( m_headTransform.GetPosition() );

	m_headTransform.SetFromMatrix( turrateBasesInTankSpace );
}

Bullet* Turret::CreateANewBullet()
{
	Bullet* newBullet = new Bullet( m_headTransform.GetWorldPosition(), m_headTransform.GetWorldTransformMatrix().GetKColumn() );
	
	return newBullet;
}
