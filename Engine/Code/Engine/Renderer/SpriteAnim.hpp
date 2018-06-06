#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

class SpriteAnim
{
public:
	 SpriteAnim( const SpriteAnimDefinition* animDef );
	~SpriteAnim();

	void			Update( float deltaSeconds );
	void			PlayFromStart();
	bool			IsFinished() const { return m_isFinished; }
	float			GetElapsedSeconds() const { return m_elapsedSeconds; }
	float			GetElapsedFraction() const; // Hint: Asks its SpriteAnimDefinition for total duration
	float			GetRemainingSeconds() const;
	float			GetRemainingFraction() const;
	const Texture&	GetTexture() const;
	AABB2			GetCurrentUVs() const;
	std::string		GetName() const;

protected:
	const SpriteAnimDefinition*		m_animDef = nullptr;
	bool							m_isPlaying = true;
	bool							m_isFinished = false;
	float							m_elapsedSeconds = 0.f;
};
