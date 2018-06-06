#pragma once
#include "VertexLayout.hpp"
#include "Engine/Core/EngineCommon.hpp"

VertexLayout::VertexLayout( unsigned int stride, VertexAttribute const *attributeStaticArray )
{
	m_stride = stride;

	for( unsigned int i = 0; !attributeStaticArray[i].name.empty(); i++ )
		m_attributes.push_back( attributeStaticArray[i] );
}

unsigned int VertexLayout::GetAttributeCount() const
{
	return (unsigned int)m_attributes.size();
}

VertexAttribute const& VertexLayout::GetAttributeAtIndex( unsigned const idx ) const
{
	return m_attributes[ idx ];
}