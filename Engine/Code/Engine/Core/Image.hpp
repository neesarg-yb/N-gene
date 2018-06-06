#pragma once
#include <string>
#include <vector>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Image
{
public:
	explicit Image( const std::string& imageFilePath, bool flipVertically = false );
	explicit Image( const Rgba& pixalColor, int width_px = 1, int height_px = 1 );

	IntVector2		GetDimensions() const;
	inline uint		GetWidth () const { return m_dimensions.x; }
	inline uint		GetHeight() const { return m_dimensions.y; }

	Rgba			GetTexel( int x, int y ) const; 				// (0,0) is top-left; ( x, y ) = ( columnNum, rowNum )
	void			SetTexel( int x, int y, const Rgba& color );	// (0,0) is top-left; ( x, y ) = ( columnNum, rowNum )

	unsigned char*		 GetPointerToTexelVector();
	unsigned char const* GetBuffer( uint offsetX, uint offsetY ) const;

private:
	IntVector2			m_dimensions;
	std::vector< Rgba >	m_texels;							// ordered left-to-right, then down, from [0]=(0,0) at top-left
};