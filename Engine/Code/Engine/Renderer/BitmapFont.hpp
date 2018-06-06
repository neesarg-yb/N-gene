#pragma once
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

class DebugRenderer;

class BitmapFont
{
	friend class Renderer;

public:
	const std::string m_fontName;

	AABB2 GetUVsForGlyph( int glyphUnicode ) const; // pass ‘A’ or 65 for A, etc.
	float GetGlyphAspect( int glyphUnicode ) const { glyphUnicode = glyphUnicode; return m_baseAspect; } // will change later
	float GetStringWidth( const std::string& asciiText, float cellHeight, float aspectScale );

	~BitmapFont();

private:
	// private, can only called by Renderer (friend class)
	explicit BitmapFont( const std::string& fontName, const SpriteSheet& glyphSheet, float baseAspect );
	
	float m_baseAspect = 1.0f;			// used as the base aspect ratio for all glyphs

public:
	const SpriteSheet&	m_spriteSheet;	// used internally; assumed to be a 16x16 glyph sheet
};