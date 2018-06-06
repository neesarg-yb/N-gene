#include "SpriteAnimDefinition.hpp"


SpriteAnimDefinition::SpriteAnimDefinition( const XMLElement& animElement, const SpriteSheet& defaultSpriteSheet, float defaultFps/*, Renderer& renderer*/ )
{
	m_name				= ParseXmlAttribute( animElement, "name", std::string("Name not set") );
	m_spriteSheet		= (SpriteSheet*) &defaultSpriteSheet;
	m_framesPerSecond	= ParseXmlAttribute( animElement, "fps", defaultFps );
	m_spriteIndexes		= ParseXmlAttribute( animElement, "spriteIndexes", m_spriteIndexes );
	m_isLooping			= ParseXmlAttribute( animElement, "looping", m_isLooping );
}

SpriteAnimDefinition::~SpriteAnimDefinition()
{

}

int SpriteAnimDefinition::GetSpriteIndexAtTime( float elapsedSeconds ) const
{
	int totalFrames = (int) m_spriteIndexes.size();
	float secondsPerFrame = 1.f / m_framesPerSecond;
	int indexOfCurrentAnimFrame = (int) ( elapsedSeconds / secondsPerFrame ) % (totalFrames);		// (currentFrame)

	return indexOfCurrentAnimFrame;
}