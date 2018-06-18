#include "OrbitCamera.hpp"

OrbitCamera::OrbitCamera( Vector3 targetPos )
	: m_target( targetPos )
{

}

OrbitCamera::~OrbitCamera()
{

}

void OrbitCamera::SetSphericalCoordinate( float rad, float rot, float alt )
{
	m_radius	= rad;
	m_rotation	= rot;
	m_altitude	= alt;

	Camera::LookAt( m_target + GetOrbitCameraPosition(), m_target );
}

void OrbitCamera::IncrementInSphericalCoordinate( float rad, float rot, float alt )
{
	SetSphericalCoordinate( m_radius + rad, m_rotation + rot, m_altitude + alt );
}

Vector3 OrbitCamera::GetOrbitCameraPosition() const
{
	return PolarToCartesian( m_radius, m_rotation, m_altitude );
}