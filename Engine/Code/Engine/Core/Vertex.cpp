#pragma once
#include "Vertex.hpp"
#include "Engine/Renderer/Renderer.hpp"

VertexAttribute const Vertex_3DPCU::s_attributes[] = 
{
	VertexAttribute( "POSITION",	RDT_FLOAT,			3,	false,	offsetof( Vertex_3DPCU, m_position	) ),
	VertexAttribute( "COLOR",		RDT_UNSIGNED_INT,	4,	true,	offsetof( Vertex_3DPCU, m_color		) ),
	VertexAttribute( "UV",			RDT_FLOAT,			2,	false,	offsetof( Vertex_3DPCU, m_UVs		) ),
	VertexAttribute( )				// like a null terminator, how do we know the list is done;
};

VertexLayout const Vertex_3DPCU::s_layout = VertexLayout( sizeof( Vertex_3DPCU ), Vertex_3DPCU::s_attributes );

Vertex_3DPCU::Vertex_3DPCU( const Vector3& position, const Rgba& color, const Vector2& UVs )
{
	m_position = position;
	m_color = color;
	m_UVs = UVs;
}