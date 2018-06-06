#pragma once
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteAnim.hpp"
#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"

class SpriteAnimSet
{
public:
	 SpriteAnimSet( SpriteAnimSetDefinition* animSetDef );
	~SpriteAnimSet();

	void 			Update( float deltaSeconds );
	void 			StartAnim( const std::string& animName );
	void 			SetCurrentAnim( const std::string& animName );
	bool			HasAnimationByName( std::string animName );
	const Texture& 	GetCurrentTexture() const;
	AABB2 			GetCurrentUVs() const;

protected:
	SpriteAnimSetDefinition*				m_animSetDef = nullptr;
	std::map< std::string, SpriteAnim* >	m_namedAnims;
	SpriteAnim*								m_currentAnim = nullptr;
};
