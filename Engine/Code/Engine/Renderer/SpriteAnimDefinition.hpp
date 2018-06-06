#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/XMLUtilities.hpp"

typedef std::vector<int> Ints;

class SpriteAnimDefinition
{
	friend class SpriteAnim;
	friend class SpriteAnimSetDefinition;

public:
	 SpriteAnimDefinition( const XMLElement& animElement, const SpriteSheet& defaultSpriteSheet, 
						   float defaultFps/*, Renderer& renderer*/ );
	~SpriteAnimDefinition();

	float	GetDuration() const { return (float) m_spriteIndexes.size() / m_framesPerSecond; }
	int		GetSpriteIndexAtTime( float elapsedSeconds ) const;

protected:
	std::string		m_name;
	SpriteSheet*	m_spriteSheet = nullptr;
	float			m_framesPerSecond = 10.f;
	Ints			m_spriteIndexes;
	bool			m_isLooping = true;
};
