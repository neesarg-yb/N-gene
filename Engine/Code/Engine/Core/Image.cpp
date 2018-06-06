#include "Image.hpp"
#include "Engine/../ThirdParty/stb/stb_image.h"

Image::Image( const std::string& imageFilePath, bool flipVertically /* = false */ )
{
	int numComponents = 0;
	int numComponentsRequested = 0;

	// Get const char* n of all the pixels
	stbi_set_flip_vertically_on_load( flipVertically );
	unsigned char* imageData = stbi_load( imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested );
	stbi_set_flip_vertically_on_load( false );

	// Get array size as per color unit
	const int totalPixals = m_dimensions.x * m_dimensions.y;
	const int componentsPerPixal = numComponents;

	// For each pixel data
	for( int i = 0; i < totalPixals * componentsPerPixal; i += componentsPerPixal )
	{
		// Get each color component
		unsigned char r = imageData[ i + 0 ];
		unsigned char g = imageData[ i + 1 ];
		unsigned char b = imageData[ i + 2 ];
		unsigned char a = componentsPerPixal != 4 ? 255 : imageData[ i + 3 ];				// Logic relies on assumption that componentsPerPixal is either 3 or 4.

		Rgba thisPixal = Rgba( r , g , b , a );

		// Add it to vector
		m_texels.push_back(thisPixal);
	}
}

Image::Image( const Rgba& pixalColor, int width_px /* = 1 */, int height_px /* = 1 */ )
{
	for( int i=0; i<width_px; i++ )
		for( int j=0; j<height_px; j++ )
			m_texels.push_back( pixalColor );

	m_dimensions = IntVector2( width_px, height_px );
}

IntVector2 Image::GetDimensions() const
{
	return m_dimensions;
}

Rgba Image::GetTexel( int x, int y ) const
{
	int index = GetIndexFromColumnRowNumberForMatrixOfWidth( x , y , m_dimensions.x );

	return m_texels[ index ];
}

void Image::SetTexel( int x, int y, const Rgba& color )
{
	int index = GetIndexFromColumnRowNumberForMatrixOfWidth( x , y , m_dimensions.x );

	m_texels[ index ] = color;
}

unsigned char * Image::GetPointerToTexelVector()
{
	return (unsigned char*) m_texels.data();
}

unsigned char const* Image::GetBuffer( uint offsetX, uint offsetY ) const
{
	uint index = GetIndexFromColumnRowNumberForMatrixOfWidth( offsetX, offsetY, m_dimensions.x );
	
	// unsigned char* buffer = (unsigned char*) m_texels.data();
	// *buffer += index;
	//		vs.
	// (unsigned char*) ( m_texels.data() + index );
	
	return (unsigned char*) ( m_texels.data() + index );
}