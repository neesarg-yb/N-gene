#include "OrbitCamera.hpp"

void OrbitCamera::SetTarget( Vector3 new_target )
{
	m_target = new_target;

	Camera::LookAt( GetOrbitCameraPosition(), m_target );
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