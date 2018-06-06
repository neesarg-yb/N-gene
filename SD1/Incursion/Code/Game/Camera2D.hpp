#pragma once 

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Vector2.hpp"

class Camera2D
{
public:
	Camera2D(float size);
	~Camera2D();

	void MoveCameraTo( const Vector2& position );
	AABB2 GetBoundsOfCurrentCameraView();
	void OverlayTextureOnScreen( Texture& overlayTexure );
	void OverlaySolidAABBOnScreenWithAlpha( const Rgba& boxColor, float alpha );

private:
	Vector2 m_worldPosition;
	float m_orientationDegrees;
	float m_numTilesInViewVertically;
	float m_aspectRatio;
};