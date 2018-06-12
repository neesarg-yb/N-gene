#pragma once
#include "TextureCube.hpp"
#include "Engine/Renderer/glfunctions.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Renderer.hpp"

// Local function
void BindImageToSide( eTextureCubeSide side, Image const &img, uint size, uint ox, uint oy, GLenum channels, GLenum pixel_layout ) 
{
	void const *ptr = img.GetBuffer( ox, oy ); 
	glTexSubImage2D( GetAsOpenGLTextureCubeSide(side),
		0,          // mip_level
		0, 0,       // offset
		size, size, 
		channels, 
		pixel_layout, 
		ptr ); 

	GL_CHECK_ERROR(); 
}

TextureCube::TextureCube()
{
	m_size	 = 0U;
	m_handle = 0U;
}

TextureCube::~TextureCube()
{
	// Cleanup: Deletes the texture
	if( IsValid() )
	{
		glDeleteTextures( 1, &m_handle );
		m_handle = 0U;
	}

	m_size = 0U;
}

bool TextureCube::MakeFromImage( char const *imagePath )
{
	// Get CubeMap texture
	Image img = Image( imagePath, false );
	TODO("When to delete the Texture? Who owns the texture?");

	uint width	= img.GetWidth();
	uint size	= width / 4;

	// Make sure it is the type/size we think it is
	GUARANTEE_OR_DIE( img.GetHeight() == (size * 3U), "Error: CubeMap isn't a 4x3 image!!" );

	// Generate handle
	if( m_handle == NULL )
	{
		glGenTextures( 1, &m_handle );
		GUARANTEE_OR_DIE( IsValid(), "Error: Could not generate a valid texture handle!" );
	}
	m_size = size;

	// Bind it
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_handle );
	glTexStorage2D( GL_TEXTURE_CUBE_MAP, 1, GetAsOpenGLTextureFormat( TEXTURE_FORMAT_RGBA8 ), m_size, m_size );
	GL_CHECK_ERROR();

	glPixelStorei( GL_UNPACK_ROW_LENGTH, img.GetWidth() );
	GL_CHECK_ERROR();

	// bind the image to the side; 
	BindImageToSide( TEXCUBE_RIGHT,  img, m_size, m_size * 2, m_size * 1, GetAsOpenGLPixelFormat( PIXEL_FORMAT_RGBA ), GL_UNSIGNED_BYTE ); 
	BindImageToSide( TEXCUBE_LEFT,   img, m_size, m_size * 0, m_size * 1, GetAsOpenGLPixelFormat( PIXEL_FORMAT_RGBA ), GL_UNSIGNED_BYTE ); 
	BindImageToSide( TEXCUBE_TOP,    img, m_size, m_size * 1, m_size * 0, GetAsOpenGLPixelFormat( PIXEL_FORMAT_RGBA ), GL_UNSIGNED_BYTE ); 
	BindImageToSide( TEXCUBE_BOTTOM, img, m_size, m_size * 1, m_size * 2, GetAsOpenGLPixelFormat( PIXEL_FORMAT_RGBA ), GL_UNSIGNED_BYTE ); 
	BindImageToSide( TEXCUBE_FRONT,  img, m_size, m_size * 1, m_size * 1, GetAsOpenGLPixelFormat( PIXEL_FORMAT_RGBA ), GL_UNSIGNED_BYTE ); 
	BindImageToSide( TEXCUBE_BACK,   img, m_size, m_size * 3, m_size * 1, GetAsOpenGLPixelFormat( PIXEL_FORMAT_RGBA ), GL_UNSIGNED_BYTE ); 

	glPixelStorei( GL_UNPACK_ROW_LENGTH, 0 ); 

	return GLSucceeded(); 
}