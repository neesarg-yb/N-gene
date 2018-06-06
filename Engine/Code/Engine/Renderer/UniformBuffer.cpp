#pragma once
#include "UniformBuffer.hpp"

UniformBuffer::UniformBuffer( size_t byteSize, void const *data )
{
	m_CPUBuffer = malloc( byteSize );
	memcpy( m_CPUBuffer, data, byteSize );

	m_renderBuffer	= new RenderBuffer();
	m_renderBuffer->CopyToGPU( byteSize, data );

	m_isDirty = false;
}

UniformBuffer::~UniformBuffer()
{
	delete m_renderBuffer;
	free( m_CPUBuffer );
}

void UniformBuffer::SetCPUData( size_t byteSize, void const *data )
{
	free( m_CPUBuffer );
	m_CPUBuffer = malloc( byteSize );
	memcpy( m_CPUBuffer, data, byteSize );

	m_isDirty = true;
}

void UniformBuffer::SetGPUData( size_t byteSize, void const *data )
{
	free( m_CPUBuffer );
	m_CPUBuffer = malloc( byteSize );
	memcpy( m_CPUBuffer, data, byteSize );

	m_renderBuffer->CopyToGPU( byteSize, data );
	m_isDirty = false;
}

void UniformBuffer::UpdateGPU()
{
	if( m_isDirty == false )
		return;

	m_renderBuffer->CopyToGPU( m_renderBuffer->buffer_size, m_CPUBuffer );
	m_isDirty = false;
}

void* UniformBuffer::GetCPUBuffer()
{
	m_isDirty = true;
	return m_CPUBuffer;
}

void const* UniformBuffer::GetCPUBuffer() const
{
	return (void const*) m_CPUBuffer;
}

size_t UniformBuffer::GetSize() const
{
	return m_renderBuffer->buffer_size;
}

GLuint UniformBuffer::GetHandle() const
{
	return m_renderBuffer->GetHandle();
}