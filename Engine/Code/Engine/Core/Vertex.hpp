#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Rgba.hpp"

struct Vertex_3DPCU
{
	Vector3 m_position;
	Rgba	m_color;
	Vector2	m_UVs;

	Vertex_3DPCU( const Vector3& position, const Rgba& color, const Vector2& UVs )
	{
		m_position = position;
		m_color = color;
		m_UVs = UVs;
	}
};