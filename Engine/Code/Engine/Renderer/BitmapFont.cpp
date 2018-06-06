#include "BitmapFont.hpp"

BitmapFont::BitmapFont( const std::string& fontName, const SpriteSheet& glyphSheet, float baseAspect )
	: m_spriteSheet( glyphSheet )
	, m_baseAspect( baseAspect )
	, m_fontName( fontName )
{

}

BitmapFont::~BitmapFont()
{

}

AABB2 BitmapFont::GetUVsForGlyph( int glyphUnicode ) const
{
	return m_spriteSheet.GetTexCoordsForSpriteIndex(glyphUnicode);
}

float BitmapFont::GetStringWidth( const std::string& asciiText, float cellHeight, float aspectScale )
{
	int totalCharacters = (int) asciiText.length();

	// aspectRatio = width / height
	// i.e. width = aspectScale * (aspectRatio * height)
	// therefor for all characters: totalWidth = width * totalCharacters
	float widthOfOneChar = (m_baseAspect * aspectScale) * cellHeight;

	return totalCharacters * widthOfOneChar;
}