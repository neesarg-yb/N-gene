#pragma once
#include "Renderable.hpp"

Renderable::~Renderable()
{
	// Delete all Meshes
	for each( Mesh* mesh in m_meshes )
		delete mesh;

	// Delete all Materials
	for each( Material* material in m_materials )
		delete material;
}

Renderable::Renderable( Mesh *mesh, Material *material )
{
	m_meshes.push_back( mesh );
	m_materials.push_back( material );
}

Renderable::Renderable(  Transform const &transform )
	: m_modelTransform( transform )
{

}

Renderable::Renderable( Transform const &transform, Mesh *mesh, Material *material )
	: m_modelTransform( transform )
{
	m_meshes.push_back( mesh );
	m_materials.push_back( material );
}

Renderable::Renderable( Vector3 const &position, Vector3 const &eulerRotation /*= Vector3::ZERO*/, Vector3 const &scale /*= Vector3::ONE_ALL */ )
	: m_modelTransform( position, eulerRotation, scale )
{

}

Mesh const* Renderable::GetMesh( uint idx /*= 0 */ ) const
{
	if( idx < m_meshes.size() )
		return m_meshes[ idx ];
	else
		return nullptr;
}

Material* Renderable::GetMaterial( uint idx /*= 0 */ )
{
	if( idx < m_materials.size() )
		return m_materials[ idx ];
	else
		return nullptr;
}

Transform const& Renderable::GetTransform() const
{
	return m_modelTransform;
}

uint Renderable::GetRenderLayer( uint idx /*= 0 */ ) const
{
	return m_materials[ idx ]->GetShader()->m_layer;
}

bool Renderable::IsAlphaQueueType( uint idx /*= 0 */ ) const
{
	return m_materials[ idx ]->GetShader()->m_isAlphaQueueType;
}

void Renderable::SetPickID( uint pickID )
{
	m_pickID = pickID;
}

void Renderable::SetBaseMesh( Mesh *newMesh )
{
	// If empty, push back new
	if( m_meshes.size() == 0 )
		m_meshes.push_back( newMesh );
	else
	{
		// Else just change the existing one
		if( m_meshes[0] != nullptr )
			delete m_meshes[0];

		m_meshes[0] = newMesh;
	}
}

void Renderable::SetBaseMaterial( Material *newMaterial )
{
	// If empty, push back new
	if( m_materials.size() == 0 )
		m_materials.push_back( newMaterial );
	else
	{
		// Else just change the existing one
		if( m_materials[0] != nullptr )
			delete m_materials[0];

		m_materials[0] = newMaterial;
	}
}

uint Renderable::AddSubMesh( Mesh *newMesh )
{
	m_meshes.push_back( newMesh );

	return (uint)m_meshes.size();
}

uint Renderable::AddSubMaterial( Material *newMaterial )
{
	m_materials.push_back( newMaterial );

	return (uint)m_materials.size();
}

void Renderable::SetPosition( Vector3 const &newPosition )
{
	m_modelTransform.SetPosition( newPosition );
}

void Renderable::SetRotation( Vector3 const &newRotation )
{
	m_modelTransform.SetRotation( newRotation );
}

void Renderable::SetScale( Vector3 const &newScale )
{
	m_modelTransform.SetScale( newScale );
}

