#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Engine/Math/MathUtil.hpp"

class Camera2D
{
public:
	Camera2D( Entity& followEntity, Map& inMap, float tilesInViewVertically, float aspectRatio );
	void PrepCameraForNewMap( Map& inMap );			// It adjusts cameraPositionBound according to the inMap

	void Update();
	void Update( Vector2 bottomLeftOrtho, Vector2 topRightOrtho );
	
	void Render();
	void OverlaySolidAABBOnScreenWithAlpha( const Rgba& boxColor, float alpha );

	AABB2	GetCameraBounds() const;

private:
	Entity& m_followMe;
	AABB2	m_cameraPositionBounds;
	float	m_numOfTilesVertically;
	float	m_aspectRatio;

	Vector2 m_bottomLeftOrtho;
	Vector2 m_topRightOrtho;
};