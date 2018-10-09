#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Quaternion.hpp"

class CameraState
{
public:
	 CameraState() { }
	 CameraState( Vector3 const &position, Quaternion const &orientation, float const &fov );
	 CameraState( Vector3 const &position, Vector3 const &eulerAngleOrientation, float const &fov );
	~CameraState();

public:
	Vector3		m_position		= Vector3::ZERO;
	Quaternion	m_orientation	= Quaternion::IDENTITY;
	float		m_fov			= 45.f;

public:
	static CameraState Interpolate( CameraState const &fromTP, CameraState const &toTP, float byFraction );
};