#pragma once
#include "Mesh.hpp"

Mesh::Mesh()
{
	
}

Mesh::~Mesh()
{
	if( m_vbo != nullptr )
		delete m_vbo;
	if( m_ibo != nullptr )
		delete m_ibo;

	m_vbo = nullptr;
	m_ibo = nullptr;
}

void Mesh::SetIndices( unsigned int count, unsigned const *indices )
{
	if( m_ibo != nullptr )
		delete m_ibo;

	m_ibo = new IndexBuffer( count, sizeof( unsigned int ), indices );
}

void Mesh::SetVertices( unsigned int count, unsigned int vertexStride, void const *data )
{
	if( m_vbo != nullptr )
		delete m_vbo;

	m_vbo = new VertexBuffer( count, vertexStride, data );
}

void Mesh::SetDrawInstruction( ePrimitiveType type, bool useIndices, unsigned int startIndex, unsigned int elementCount )
{
	m_drawCallInstruction = DrawInstruction( type, startIndex, elementCount, useIndices );
}

void Mesh::SetDrawInstruction( DrawInstruction const & drawInstruction )
{
	m_drawCallInstruction = drawInstruction;
}