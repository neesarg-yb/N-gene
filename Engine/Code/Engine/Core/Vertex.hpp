#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/VertexLayout.hpp"

struct Vertex_3DPCU
{
	Vector3 m_position	= Vector3::ZERO;
	Rgba	m_color		= RGBA_WHITE_COLOR;
	Vector2	m_UVs		= Vector2::ZERO;

	static VertexAttribute const s_attributes[];
	static VertexLayout	   const s_layout;

	Vertex_3DPCU() {};
	Vertex_3DPCU( const Vector3& position, const Rgba& color, const Vector2& UVs );
};