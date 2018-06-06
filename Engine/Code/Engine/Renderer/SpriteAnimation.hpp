#pragma once
#include "Engine/Renderer/SpriteSheet.hpp"

enum SpriteAnimMode
{
	SPRITE_ANIM_MODE_PLAY_TO_END,	// Play from time=0 to durationSeconds, then finish
	SPRITE_ANIM_MODE_LOOPING,		// Play from time=0 to end then repeat (never finish)
	NUM_SPRITE_ANIM_MODES
};

class SpriteAnimation
{
public:
	// Data
	const SpriteSheet&	m_spriteSheet;
	SpriteAnimMode		m_playbackMode;
	float				m_durationSeconds;
	int					m_startSpriteIndex;
	int					m_endSpriteIndex;

	// State
	bool				m_isPlaying;
	bool				m_isFinished;
	float				m_elapsedSeconds;

	SpriteAnimation( const SpriteSheet& spriteSheet, float durationSeconds, SpriteAnimMode playbackMode, int startSpriteIndex, int endSpriteIndex );
	
	void			Update( float deltaSeconds );
	AABB2			GetCurrentTexCoords() const;	// Based on the current elapsed time
	const Texture&  GetTexture() const;
	void	Pause();						// Starts unpaused (playing) by default
	void	Resume();						// Resume after pausing
	void	Reset();						// Rewinds to time 0 and starts (re)playing
	bool	IsFinished() const;
	bool	IsPlaying() const;
	float	GetDurationSeconds() const;
	float	GetSecondsElapsed() const;
	float	GetSecondsRemaining() const;
	float	GetFractionElapsed() const;
	float	GetFractionRemaining() const;
	void	SetSecondsElapsed( float secondsElapsed );	    // Jump to specific time
	void	SetFractionElapsed( float fractionElapsed );    // e.g. 0.33f for one-third in
};