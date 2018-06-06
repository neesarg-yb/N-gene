#include "Camera.hpp"


Camera2D::Camera2D(float size)
{
	m_numTilesInViewVertically = size;
	m_aspectRatio = 16 / 9;
}

Camera2D::~Camera2D()
{

}

void Camera2D::MoveCameraTo( const Vector2& position ) {
	float newWidth = m_numTilesInViewVertically * m_aspectRatio;
	float newHeight = m_numTilesInViewVertically;

	g_theRenderer->SetOrtho(position - Vector2(newWidth/2, newHeight/2), position + Vector2(newWidth/2, newHeight/2));
}