#pragma once
#include "DrawCall.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/Transform.hpp"

DrawCall::DrawCall( Mesh const &mesh, Material &material, Transform &transform )
{
	m_mesh		= &mesh;
	m_material	= &material;
	m_model		= transform.GetWorldTransformMatrix();
}

