#pragma once
#include <map>
#include <string>
#include "Engine/Core/Image.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Renderer/RenderTypes.hpp"

class Texture
{
	friend class Renderer;							// Textures are managed by a Renderer instance

public:
	unsigned int GetHandle() const;
	unsigned int GetWidth () const;
	unsigned int GetHeight() const;

public:
	~Texture();

private:
	 Texture() {};
	 Texture( const std::string& imageFilePath );	// Use renderer->CreateOrGetTexture() instead!
	 Texture( Image& image );						// Creates a Texture from Image object

	void PopulateFromData	( unsigned char* imageData, const IntVector2& texelSize, int numComponents );
	bool CreateRenderTarget	( unsigned int width, unsigned int height, eTextureFormat fmt );

private:
	unsigned int	m_textureID;
	IntVector2		m_dimensions	= IntVector2( 0, 0 );
	eTextureFormat	m_format		= TEXTURE_FORMAT_RGBA8;
};