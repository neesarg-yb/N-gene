#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Matrix44.hpp"

class Mesh;
class Material;
class Renderable;
class Transform;

class DrawCall
{
public:
	DrawCall() { };
	DrawCall( Mesh const &mesh, Material &material, Transform &transform );		// Sets up the model, mesh & material

public:
	// Rendering details
	Matrix44		 m_model;
	Mesh const		*m_mesh				= nullptr;
	Material		*m_material			= nullptr;

	// Lights
	unsigned int	 m_lightCount		= 0;
	unsigned int	 m_lightIndices[ MAX_LIGHTS ];
	
	// Layers
	unsigned int	 m_layer			= 0;
	bool			 m_queueTypeIsApha	= false;
};