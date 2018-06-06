#include "Camera2D.hpp"
#include "Game/GameCommon.hpp"


Camera2D::Camera2D(float size)
{
	m_numTilesInViewVertically = size;
	m_aspectRatio = 16 / 9;
}

Camera2D::~Camera2D()
{

}

void Camera2D::MoveCameraTo( const Vector2& position ) {
	m_worldPosition = position;

	float newWidth = m_numTilesInViewVertically * m_aspectRatio;
	float newHeight = m_numTilesInViewVertically;

	g_theRenderer->SetOrtho(position - Vector2(newWidth/2, newHeight/2), position + Vector2(newWidth/2, newHeight/2));
}

AABB2 Camera2D::GetBoundsOfCurrentCameraView()
{
	float width = m_numTilesInViewVertically * m_aspectRatio;
	float height = m_numTilesInViewVertically;
	AABB2 bounds = AABB2(m_worldPosition , width/2.0f , height/2.0f);

	return bounds;
}

void Camera2D::OverlayTextureOnScreen( Texture& overlayTexure )
{
	AABB2 bounds = GetBoundsOfCurrentCameraView();
	g_theRenderer->DrawTexturedAABB( bounds , overlayTexure , Vector2(0,1) , Vector2(1,0) , RGBA_WHITE_COLOR /*Rgba( 255, 255, 255, 200)*/);
}

void Camera2D::OverlaySolidAABBOnScreenWithAlpha( const Rgba& boxColor, float alpha )
{
	alpha = MathUtil::ClampFloat(alpha, 0 , 255);

	AABB2 bounds = GetBoundsOfCurrentCameraView();
	Rgba colorWithAlpha = Rgba( boxColor.r , boxColor.g , boxColor.b , (unsigned char)alpha );
	g_theRenderer->DrawAABB(bounds , colorWithAlpha);
}