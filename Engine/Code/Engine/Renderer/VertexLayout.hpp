#pragma once
#include <string>
#include <vector>
#include "Engine/Core/EngineCommon.hpp"

enum eRenderDataType;

struct VertexAttribute
{
	std::string				name;
	eRenderDataType			type;
	unsigned int			elementsCount;
	bool					normalize;
	unsigned int			memberOffset;

	VertexAttribute() { };
	VertexAttribute( std::string name, eRenderDataType dataType, unsigned int elemCount, bool normalize, unsigned int memberOffset )
	{
		this->name			= name;
		this->type			= dataType;
		this->elementsCount	= elemCount;
		this->normalize		= normalize;
		this->memberOffset	= memberOffset;
	}
};

class VertexLayout
{
public:
	VertexLayout( unsigned int stride, VertexAttribute const *attributeStaticArray );

public:
	std::vector< VertexAttribute >	m_attributes;
	unsigned int					m_stride;

public:
	unsigned int			GetAttributeCount() const;
	VertexAttribute	const&	GetAttributeAtIndex( unsigned const idx ) const;
};