#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer( unsigned int vertexCount, unsigned int vertexStride, void const *data )
	: m_vertexCount( vertexCount )
	, m_vertexStride( vertexStride )
{
	unsigned int byteCount = m_vertexCount * m_vertexStride;
	CopyToGPU( byteCount, data );
}

VertexBuffer::~VertexBuffer()
{

}