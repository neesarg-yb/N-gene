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

VertexAttribute const Vertex_Lit::s_attributes[] =
{
	VertexAttribute("POSITION",		RDT_FLOAT,			3,	false,	offsetof(Vertex_Lit, m_position	) ),
	VertexAttribute("COLOR",		RDT_UNSIGNED_INT,	4,	true,	offsetof(Vertex_Lit, m_color	) ),
	VertexAttribute("UV",			RDT_FLOAT,			2,	false,	offsetof(Vertex_Lit, m_UVs		) ),
	VertexAttribute("NORMAL",		RDT_FLOAT,			3,	false,	offsetof(Vertex_Lit, m_normal	) ),
	VertexAttribute("TANGENT",		RDT_FLOAT,			4,	false,	offsetof(Vertex_Lit, m_tangent	) ),
	VertexAttribute( )				// like a null terminator, how do we know the list is done;
};

VertexLayout const Vertex_3DPCU::s_layout	= VertexLayout(sizeof(Vertex_3DPCU),	Vertex_3DPCU::s_attributes);
VertexLayout const   Vertex_Lit::s_layout	= VertexLayout(sizeof(Vertex_Lit),		  Vertex_Lit::s_attributes);
