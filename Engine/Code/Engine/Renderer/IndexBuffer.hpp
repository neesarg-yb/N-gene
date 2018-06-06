#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"

class IndexBuffer : public RenderBuffer
{
public:
	 IndexBuffer( unsigned int indexCount, unsigned int indexStride, void const * data );
	~IndexBuffer();

public:
	unsigned int m_indexCount;
	unsigned int m_indexStride;
};