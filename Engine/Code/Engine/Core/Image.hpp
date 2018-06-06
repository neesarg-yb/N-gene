#pragma once
#include <string>
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVector2.hpp"

class Image
{
public:
	explicit Image( const std::string& imageFilePath );

	IntVector2 GetDimensions() const;

	Rgba	GetTexel( int x, int y ) const; 				// (0,0) is top-left; ( x, y ) = ( columnNum, rowNum )
	void	SetTexel( int x, int y, const Rgba& color );	// (0,0) is top-left; ( x, y ) = ( columnNum, rowNum )

private:
	IntVector2		m_dimensions;
	std::vector< Rgba >	m_texels;							// ordered left-to-right, then down, from [0]=(0,0) at top-left
};