#include "Engine/Renderer/SpriteAnimation.hpp"

SpriteAnimation::SpriteAnimation( const SpriteSheet& spriteSheet, float durationSeconds, SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex )
	: m_spriteSheet(spriteSheet)
{
	m_durationSeconds = durationSeconds;
	m_playbackMode = playbackMode;
	m_startSpriteIndex = startSpriteIndex;
	m_endSpriteIndex = endSpriteIndex;

	m_isPlaying = true;
	m_isFinished = false;
	m_elapsedSeconds = 0.f;
}

void SpriteAnimation::Update( float deltaSeconds )
{
	// If animation is not playing or is finished, return
	if( m_isPlaying == false || m_isFinished == true )
		return;

	// If animation is playing, and is started 
	if( m_isPlaying == true )
	{
		// Update delta seconds
		m_elapsedSeconds += deltaSeconds;

		// If playback is PLAY_TO_END, stop animation once finished
		if( m_playbackMode == SPRITE_ANIM_MODE_PLAY_TO_END )
		{
			if( m_elapsedSeconds >= m_durationSeconds )
			{
				m_isFinished = true;
				m_isPlaying = false;

				m_elapsedSeconds -= deltaSeconds;	// if animation is complete, do not add this frame's deltaSeconds
			}
		}
	}
}

AABB2 SpriteAnimation::GetCurrentTexCoords() const
{
	int totalFrames = m_endSpriteIndex - m_startSpriteIndex + 1;
	float timePerAnimFrame = m_durationSeconds / totalFrames;			// (duration / length)
	int   indexOfCurrentAnimFrame = m_startSpriteIndex + ( (int) ( m_elapsedSeconds / timePerAnimFrame ) % (totalFrames) );		// (startIndex + currentFrame)

	return m_spriteSheet.GetTexCoordsForSpriteIndex(indexOfCurrentAnimFrame);
}

const Texture& SpriteAnimation::GetTexture() const
{
	return m_spriteSheet.m_spriteSheetTexture;
}

void SpriteAnimation::Pause()
{
	m_isPlaying = false;
}

void SpriteAnimation::Resume()
{
	m_isPlaying = true;
}

void SpriteAnimation::Reset()
{
	m_elapsedSeconds = 0.f;
	m_isPlaying = true;
	m_isFinished = false;
}

bool SpriteAnimation::IsPlaying() const
{
	return m_isPlaying;
}

bool SpriteAnimation::IsFinished() const
{
	return m_isFinished;
}

float SpriteAnimation::GetDurationSeconds() const
{
	return m_durationSeconds;
}

float SpriteAnimation::GetSecondsElapsed() const
{
	return m_elapsedSeconds;
}

float SpriteAnimation::GetSecondsRemaining() const
{
	return m_durationSeconds - m_elapsedSeconds;
}

float SpriteAnimation::GetFractionElapsed() const
{
	return ( m_elapsedSeconds/m_durationSeconds );
}

float SpriteAnimation::GetFractionRemaining() const
{
	return 1.f - GetFractionElapsed();
}

void SpriteAnimation::SetSecondsElapsed( float secondsElapsed )
{
	m_elapsedSeconds = secondsElapsed;
}

void SpriteAnimation::SetFractionElapsed( float fractionElapsed )
{
	float secondsElasped = m_durationSeconds * fractionElapsed;
	m_elapsedSeconds = secondsElasped;
}