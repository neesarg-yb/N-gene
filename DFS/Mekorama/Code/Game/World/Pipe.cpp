#pragma once
#include "Pipe.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

Pipe::Pipe( Vector3 const &startPos, ePipeForwardDirection pipeForward, float length )
	: GameObject( GAMEOBJECT_TYPE_PIPE )
{
	// Setup the transform
	Vector3 forwardDir	= Pipe::GetDirectionAsVector3( pipeForward );
	Vector3 position	= startPos + ( forwardDir * length * 0.5f );
	Vector3 rotation	= GetRotationFromForward( pipeForward );
	m_transform.SetPosition( position );
	m_transform.SetRotation( rotation );

	// Setup the Renderable
	float radius = 0.15f;
	Vector3 meshStartPos	= Vector3( 0.f, 0.f, length * -0.5f );
	Vector3 meshEndPos		= meshStartPos * -1.f;
	MeshBuilder mbCylinder;
	mbCylinder.Begin( PRIMITIVE_TRIANGES, true );
	mbCylinder.AddSphere( radius, 10, 10, meshStartPos );
	mbCylinder.AddCylinder( radius, 20, length, Vector3::ZERO, RGBA_WHITE_COLOR );
	mbCylinder.AddSphere( radius, 10, 10, meshEndPos );
	mbCylinder.End();

	Material *material	= Material::CreateNewFromFile( "Data\\Materials\\Pipe.material" );
	m_renderable		= new Renderable( mbCylinder.ConstructMesh<Vertex_Lit>(), material );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );
}

Pipe::~Pipe()
{

}

void Pipe::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Pipe::ObjectSelected()
{

}

Vector3 Pipe::GetDirectionAsVector3( ePipeForwardDirection pipeDir )
{
	switch (pipeDir)
	{
	case PIPE_DIR_WORLD_FRONT:
		return Vector3( 0.f, 0.f, 1.f );

	case PIPE_DIR_WORLD_BACK:
		return Vector3( 0.f, 0.f, -1.f );
	
	case PIPE_DIR_WORLD_LEFT:
		return Vector3( -1.f, 0.f, 0.f );
	
	case PIPE_DIR_WORLD_RIGHT:
		return Vector3( 1.f, 0.f, 0.f );
	
	case PIPE_DIR_WORLD_UP:
		return Vector3( 0.f, 1.f, 0.f );
	
	case PIPE_DIR_WORLD_DOWN:
		return Vector3( 0.f, -1.f, 0.f );
	default:
		GUARANTEE_OR_DIE( false, "Pipe: Invalid forward direction!! " );
		return Vector3( 0.f, 0.f, 1.f );
	}
}

Vector3 Pipe::GetRotationFromForward( ePipeForwardDirection forward )
{
	switch (forward)
	{
	case PIPE_DIR_WORLD_FRONT:
		return Vector3( 0.f, 0.f, 0.f );

	case PIPE_DIR_WORLD_BACK:
		return Vector3( 0.f, 180.f, 0.f );

	case PIPE_DIR_WORLD_LEFT:
		return Vector3( 0.f, -90.f, 0.f );

	case PIPE_DIR_WORLD_RIGHT:
		return Vector3( 0.f, 90.f, 0.f );

	case PIPE_DIR_WORLD_UP:
		return Vector3( -90.f, 0.f, 0.f );

	case PIPE_DIR_WORLD_DOWN:
		return Vector3( 90.f, 0.f, 0.f );

	default:
		GUARANTEE_OR_DIE( false, "Pipe: Invalid forward direction!! " );
		return Vector3::ZERO;
	}
}