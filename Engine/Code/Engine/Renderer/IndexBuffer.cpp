#include "IndexBuffer.hpp"

IndexBuffer::IndexBuffer( unsigned int indexCount, unsigned int indexStride, void const * data )
	: m_indexCount( indexCount )
	, m_indexStride( indexStride )
{
	unsigned int byteCount = indexCount * indexStride;
	CopyToGPU( byteCount, data );
}

IndexBuffer::~IndexBuffer()
{

}