#include "SpriteAnim.hpp"

SpriteAnim::SpriteAnim( const SpriteAnimDefinition* animDef )
{
	m_animDef = animDef;

}

SpriteAnim::~SpriteAnim()
{

}

void SpriteAnim::Update( float deltaSeconds )
{
	// If animation is not playing or is finished, return
	if( m_isPlaying == false || m_isFinished == true )
		return;

	if( m_isPlaying == true )
	{
		// Update delta seconds
		m_elapsedSeconds += deltaSeconds;

		// If looping is turned off, stop animation once finished
		if( m_animDef->m_isLooping != true )
		{
			if( m_elapsedSeconds >= m_animDef->GetDuration() )
			{
				m_isFinished = true;
				m_isPlaying = false;

				m_elapsedSeconds -= deltaSeconds;		// if animation is complete, do not add this frame's deltaSeconds
			}
		}
	}
}

void SpriteAnim::PlayFromStart()
{
	m_elapsedSeconds = 0.f;
	m_isFinished = false;
	m_isPlaying = true;
}

float SpriteAnim::GetElapsedFraction() const // Hint: Asks its SpriteAnimDefinition for total duration
{
	return m_elapsedSeconds / m_animDef->GetDuration();
}

float SpriteAnim::GetRemainingSeconds() const
{
	return m_animDef->GetDuration() - m_elapsedSeconds;
}

float SpriteAnim::GetRemainingFraction() const
{
	return GetRemainingSeconds() / m_animDef->GetDuration();
}

const Texture& SpriteAnim::GetTexture() const
{
	return m_animDef->m_spriteSheet->m_spriteSheetTexture;
}

AABB2 SpriteAnim::GetCurrentUVs() const
{
	int		totalFrames = m_animDef->m_spriteIndexes.size();
	float	secondPerFrame = 1 / m_animDef->m_framesPerSecond;			// (duration / length)
	int		indexOfCurrentAnimFrame = (int) ( m_elapsedSeconds / secondPerFrame ) % (totalFrames);		// (currentFrame)

	return m_animDef->m_spriteSheet->GetTexCoordsForSpriteIndex( m_animDef->m_spriteIndexes.at(indexOfCurrentAnimFrame) );
}

std::string	SpriteAnim::GetName() const
{
	return m_animDef->m_name;
}