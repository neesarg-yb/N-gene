#pragma once
#include <vector>
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Core/EngineCommon.hpp"

#define INVALID_PICK_ID 0U

class Renderable
{
public:
	~Renderable();
	 Renderable() { }
	 Renderable( Mesh *mesh, Material *material );
	 Renderable( Transform const &transform );
	 Renderable( Transform const &transform, Mesh *mesh, Material *material );
	 Renderable( Vector3 const &position, Vector3 const &eulerRotation = Vector3::ZERO, Vector3 const &scale = Vector3::ONE_ALL );

public:
	uint						m_pickID			= 4294967295;
	Transform					m_modelTransform;
	std::vector< Mesh* >		m_meshes;
	std::vector< Material* >	m_materials;

public:
	// Getters
	Material*			GetMaterial	( uint idx = 0 );
	Mesh		const*	GetMesh		( uint idx = 0 ) const;
	Transform	const&	GetTransform()	const;
	inline		uint	GetPickID()		const { return m_pickID; }

	uint				GetRenderLayer	( uint idx = 0 ) const;
	bool				IsAlphaQueueType( uint idx = 0 ) const;

	// Quick inline(s)
	inline Vector3		GetPosition() const { return m_modelTransform.GetWorldPosition(); }
	inline Vector3		GetRotation() const { return m_modelTransform.GetRotation(); }
	inline Vector3		GetScale   () const { return m_modelTransform.GetScale();    }

	// Setters
	void	SetPickID		( uint pickID );
	void	SetBaseMesh		( Mesh *newMesh );
	void	SetBaseMaterial	( Material *newMaterial );
	uint	AddSubMesh		( Mesh *newMesh );								// Adds a new mesh to the Renderable;     returns count of total number of Meshes, now
	uint	AddSubMaterial	( Material *newMaterial );						// Adds a new material to the Renderable; returns count of total number of Materials, now
	
	void	SetPosition	( Vector3 const &newPosition );
	void	SetRotation	( Vector3 const &newRotation );
	void	SetScale	( Vector3 const &newScale );
};