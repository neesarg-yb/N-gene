#pragma once 

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Game/GameCommon.hpp"

class Camera2D
{
public:
	Camera2D(float size);
	~Camera2D();

	void MoveCameraTo( const Vector2& position );

private:
	Vector2 m_worldPosition;
	float m_orientationDegrees;
	float m_numTilesInViewVertically;
	float m_aspectRatio;
};