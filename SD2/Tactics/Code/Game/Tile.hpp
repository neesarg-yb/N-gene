#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TileDefinition.hpp"

class Tile
{

public:
	 Tile( const Vector3 worldPosition, TileDefinition* definition );
	~Tile();
	
public:
	void Update( float deltaSeconds );
	void Render() const;
	void RenderInNewTint( const Rgba newTint) const;

	TileDefinition*	m_definition	= nullptr;
	Vector3			m_position		= Vector3::ZERO;

private:

};