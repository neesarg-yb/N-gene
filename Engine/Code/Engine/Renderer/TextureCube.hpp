#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderTypes.hpp"

class Renderer;

class TextureCube
{
public:
	 TextureCube();
	~TextureCube();

public:
	bool MakeFromImage( char const *imagePath );

	inline uint GetWidth () const { return m_size; }
	inline uint GetHeight() const { return m_size; }

	inline bool IsValid() const { return m_handle != 0U; }

public:
	uint m_size;
	uint m_handle;
};