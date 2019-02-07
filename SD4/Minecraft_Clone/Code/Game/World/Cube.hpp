#pragma once
#include "Engine/Renderer/MeshBuilder.hpp"

class Cube 
{
public:
	 Cube( Vector3 const &center );
	~Cube();

public:
	Mesh	*m_mesh			= nullptr;
	AABB3	 m_wordBounds;

public:
	static Mesh* ConstructMesh( Vector3 const &center, Vector3 const &size, AABB2 const &uvSide = AABB2::ONE_BY_ONE, AABB2 const &uvBottom = AABB2::ONE_BY_ONE, AABB2 const &uvTop = AABB2::ONE_BY_ONE, Rgba const &color = RGBA_WHITE_COLOR );
	
};
