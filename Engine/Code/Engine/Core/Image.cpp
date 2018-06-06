#include "Image.hpp"
#include "Engine/../ThirdParty/stb/stb_image.h"

Image::Image( const std::string& imageFilePath )
{
	int numComponents = 0;
	int numComponentsRequested = 0;

	// Get const char* n of all the pixels
	unsigned char* imageData = stbi_load( imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested );
	
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