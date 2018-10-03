#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Quaternion.hpp"

class CameraDestination
{
public:
	 CameraDestination() { }
	 CameraDestination( Vector3 const &position, Quaternion const &orientation, float const &fov );
	 CameraDestination( Vector3 const &position, Vector3 const &eulerAngleOrientation, float const &fov );
	~CameraDestination();

public:
	Vector3		m_position		= Vector3::ZERO;
	Quaternion	m_orientation	= Quaternion::IDENTITY;
	float		m_fov			= 45.f;

public:
	static CameraDestination Interpolate( CameraDestination const &fromTP, CameraDestination const &toTP, float byFraction );
};