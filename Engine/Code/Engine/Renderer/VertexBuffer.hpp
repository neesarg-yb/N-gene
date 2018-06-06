#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"

class VertexBuffer : public RenderBuffer
{
public:
	 VertexBuffer( unsigned int vertexCount, unsigned int vertexStride, void const *data );
	~VertexBuffer();

public:
	unsigned int m_vertexCount	= 0;
	unsigned int m_vertexStride	= 0;
};