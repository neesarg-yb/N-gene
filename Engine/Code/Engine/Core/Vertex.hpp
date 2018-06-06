#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/VertexLayout.hpp"

struct Vertex_Master
{
	Vector3	m_position	= Vector3::ZERO;
	Rgba	m_color		= RGBA_WHITE_COLOR;
	Vector2	m_UVs		= Vector2::ZERO;
	Vector3	m_normal	= Vector3( 0.f, 0.f, -1.f );
	Vector4 m_tangent	= Vector4( 1.f, 0.f, 0.f, 1.f );		// w = 1 means bitangent on right side; w = -1 means bitangent on left side
	
	// Constructors
	Vertex_Master() {}
	Vertex_Master( const Vector3 &position, const Rgba &color, const Vector2 &UVs, const Vector3 &normal, const Vector4 &tangent )
	{
		m_position	= position;
		m_color		= color;
		m_UVs		= UVs;
		m_normal	= normal;
		m_tangent	= tangent;
	}
};

struct Vertex_3DPCU
{
	Vector3 m_position	= Vector3::ZERO;
	Rgba	m_color		= RGBA_WHITE_COLOR;
	Vector2	m_UVs		= Vector2::ZERO;

	// Layout & Attributes
	static VertexAttribute const s_attributes[];
	static VertexLayout	   const s_layout;
	
	// Constructors
	Vertex_3DPCU() {}
	Vertex_3DPCU( const Vector3& position, const Rgba& color, const Vector2& UVs )
	{
		m_position = position;
		m_color = color;
		m_UVs = UVs;
	}
	Vertex_3DPCU( Vertex_Master const &other )
	{
		m_position		= other.m_position;
		m_color			= other.m_color;
		m_UVs			= other.m_UVs;
	}
};

struct Vertex_Lit
{
	Vector3	m_position	= Vector3::ZERO;
	Rgba	m_color		= RGBA_WHITE_COLOR;
	Vector2	m_UVs		= Vector2::ZERO;
	Vector3	m_normal	= Vector3( 0.f, 0.f, -1.f );
	Vector4 m_tangent	= Vector4( 1.f, 0.f, 0.f, 1.f );		// w = 1 means bitangent on right side; w = -1 means bitangent on left side
	
	// Layout & Attributes
	static VertexAttribute	const s_attributes[];
	static VertexLayout		const s_layout;

	// Constructors
	Vertex_Lit() {}
	Vertex_Lit( const Vector3 &position, const Rgba &color, const Vector2 &UVs, const Vector3 &normal, const Vector4 &tangent )
	{
		m_position	= position;
		m_color		= color;
		m_UVs		= UVs;
		m_normal	= normal;
		m_tangent	= tangent;
	}
	Vertex_Lit( Vertex_Master const &other )
	{
		m_position		= other.m_position;
		m_color			= other.m_color;
		m_UVs			= other.m_UVs;
		m_normal		= other.m_normal;
		m_tangent		= other.m_tangent;
	}
};