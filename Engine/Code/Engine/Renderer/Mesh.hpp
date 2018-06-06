#pragma once
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderTypes.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/VertexLayout.hpp"

struct DrawInstruction
{
	ePrimitiveType	primitiveType	= PRIMITIVE_TRIANGES;
	unsigned int	startIndex		= 0;
	unsigned int	elementCount	= 0;
	bool			isUsingIndices	= true;

	DrawInstruction() {};
	DrawInstruction( ePrimitiveType primitiveType, unsigned int startIndex, unsigned int elementCount, bool isUsingIndices )
	{
		this->primitiveType		= primitiveType;
		this->startIndex		= startIndex;
		this->elementCount		= elementCount;
		this->isUsingIndices	= isUsingIndices;
	}
};

class Mesh
{
public:
	 Mesh();
	~Mesh();

public:
	VertexBuffer		*m_vbo		= nullptr;
	IndexBuffer			*m_ibo		= nullptr;
	DrawInstruction		 m_drawCallInstruction;

public:
	VertexLayout const	*m_layout;
	unsigned int		GetVertexStride() const { return m_layout->m_stride; }

public:
	template <typename VERTTYPE>
	void SetVertices( unsigned int count, VERTTYPE const *vertices )
	{
		SetVertices( count, sizeof(VERTTYPE), vertices );
		m_layout = &VERTTYPE::s_layout;
	}

public:
	void SetIndices(  unsigned int count, unsigned const *indices );
	void SetDrawInstruction( ePrimitiveType type, bool useIndices, unsigned int startIndex, unsigned int elementCount );
	void SetDrawInstruction( DrawInstruction const & drawInstruction );

private:
	void SetVertices( unsigned int count, unsigned int vertexStride, void const *data );
};