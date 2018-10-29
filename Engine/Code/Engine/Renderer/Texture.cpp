//-----------------------------------------------------------------------------------------------
// Texture.cpp
//
#include "Engine/Internal/WindowsCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Renderer/External/glcorearb.h"
#include "Engine/Renderer/glfunctions.hpp"


//-----------------------------------------------------------------------------------------------
// Called only by the Renderer.  Use renderer->CreateOrGetTexture() to instantiate textures.
//
Texture::Texture( const std::string& imageFilePath )
	: m_textureID( 0 )
	, m_dimensions( 0, 0 )
{
	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)

	// For newer context of OpenGL ( Upgraded from Semester II )
	stbi_set_flip_vertically_on_load( true );	// Flip the Vs of UV coordinate so that (0, 0) starts from bottom-left, instead of top-left..

	// Load (and decompress) the image RGB(A) bytes from a file on disk, and create an OpenGL texture instance from it
	unsigned char* imageData = stbi_load( imageFilePath.c_str(), &m_dimensions.x, &m_dimensions.y, &numComponents, numComponentsRequested );
	PopulateFromData( imageData, m_dimensions, numComponents );
	stbi_image_free( imageData );

	stbi_set_flip_vertically_on_load( false );
}

Texture::Texture( Image& image )
	: m_textureID( 0 )
	, m_dimensions( image.GetDimensions() )
{
	unsigned char* imageBuffer = image.GetPointerToTexelVector();
	PopulateFromData( (unsigned char*)imageBuffer, m_dimensions, 4 );
}


Texture::~Texture()
{
	if( m_textureID > 0 )
		glDeleteTextures( m_mipCount, &m_textureID );

	GL_CHECK_ERROR(); 
}

unsigned int Texture::GetHandle() const
{
	return m_textureID;
}

unsigned int Texture::GetWidth() const
{
	return (unsigned int)m_dimensions.x;
}

unsigned int Texture::GetHeight() const
{
	return (unsigned int)m_dimensions.y;
}

//-----------------------------------------------------------------------------------------------
// Creates a texture identity on the video card, and populates it with the given image texel data
//
void Texture::PopulateFromData( unsigned char* imageData, const IntVector2& texelSize, int numComponents )
{
	m_dimensions = texelSize;
	m_mipCount = CalculateMipCount( m_dimensions );

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, (GLuint*) &m_textureID );

	// Copy the texture - first, get use to be using texture unit 0 for this;
	glActiveTexture( GL_TEXTURE0 );
	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, m_textureID ); 
	
	GLenum channels			= GL_RGBA;
	GLenum internalFormat	= GL_RGBA8;
	GLenum pixelLayout		= GL_UNSIGNED_BYTE;
	if( numComponents == 3 )
	{
		channels		= GL_RGB;
		internalFormat	= GL_RGB8;
		pixelLayout		= GL_UNSIGNED_BYTE;
	}

	glTexStorage2D( GL_TEXTURE_2D,
		m_mipCount,							// number of levels (mip-layers)
		internalFormat,						// how is the memory stored on the GPU
		m_dimensions.x, m_dimensions.y );	// dimensions
											// copies CPU memory to the GPU - needed for texture resources
	glTexSubImage2D( GL_TEXTURE_2D,
		0,									// mip layer we're copying to
		0, 0,								// offset
		m_dimensions.x, m_dimensions.y,		// dimensions
		channels,							// which channels exist in the CPU buffer
		pixelLayout,						// how are those channels stored
		imageData );						// cpu buffer to copy;

	GL_CHECK_ERROR();

	// Generate the mip chain (generates higher layers from layer 0 loaded above)
	glActiveTexture( GL_TEXTURE0 ); 
	glBindTexture( GL_TEXTURE_2D, m_textureID );    // bind our texture to our current texture unit (0)
	glGenerateMipmap( GL_TEXTURE_2D ); 

	GL_CHECK_ERROR(); 
}

bool Texture::CreateRenderTarget( unsigned int width, unsigned int height, eTextureFormat fmt )
{
	// generate the link to this texture
	glGenTextures( 1, &m_textureID );
	if (m_textureID == NULL) {
		return false; 
	}
	
	GLenum internal_format = GL_RGBA8;  
	if (fmt == TEXTURE_FORMAT_D24S8)
		internal_format = GL_DEPTH24_STENCIL8;

	// Copy the texture - first, get use to be using texture unit 0 for this; 
	glActiveTexture( GL_TEXTURE0 ); 
	glBindTexture( GL_TEXTURE_2D, m_textureID );		// bind our texture to our current texture unit (0)
	
	// Create the GPU buffer
	glTexStorage2D( GL_TEXTURE_2D,
		1,               // number of levels (mip-layers)
		internal_format, // how is the memory stored on the GPU
		width, height ); // dimensions

	// make sure it succeeded
	GL_CHECK_ERROR(); 
	
	// cleanup after myself; 
	glBindTexture( GL_TEXTURE_2D, NULL ); // unset it; 

	// Save this all off
	m_dimensions = IntVector2( width, height );

	m_format = fmt; // I save the format with the texture
					// for sanity checking.

					// great, success
	return true; 
}

int Texture::CalculateMipCount( IntVector2 dimesions )
{
	// Calculate how many layers you need so that 2^mip_count > MaximumDimension
	
	int maxDimension = (dimesions.x >= dimesions.y) ? dimesions.x : dimesions.y;
	int mipCount	 = 0;
	while(maxDimension > 0)
	{
		mipCount++;
		maxDimension >>= 1;
	}

	return mipCount;
}
