#pragma once
#include <string>
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/GameCommon.hpp"

class Sprite
{
private:
	 Sprite( const XMLElement& spriteElement );
	~Sprite();

private:
	std::string		m_idName			= "";
	Texture*		m_texture			= nullptr;
	float			m_PPU				= 1.f;
	AABB2			m_UVsInPixels		= AABB2( 0.f, 0.f, 1.f, 1.f );
	AABB2			m_UVsNormalized		= AABB2( 0.f, 0.f, 1.f, 1.f );
	Vector2			m_pivotPoint		= Vector2::ZERO;
	Vector2			m_worldDimension	= Vector2::ZERO;

public:
	static std::map< std::string, Sprite* >	s_loadedSprites;

public:
	void Render( Matrix44 const &transformMatrix ) const;

public:
	static void LoadSpritesFromXML( const char* xmlFileName );
	static void DeleteAllSprites();
};