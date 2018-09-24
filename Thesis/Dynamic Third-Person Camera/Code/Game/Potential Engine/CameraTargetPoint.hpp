#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Quaternion.hpp"

class CameraTargetPoint
{
public:
	 CameraTargetPoint() { }
	 CameraTargetPoint( Vector3 const &position, Quaternion const &orientation, float const &fov );
	 CameraTargetPoint( Vector3 const &position, Vector3 const &eulerAngleOrientation, float const &fov );
	~CameraTargetPoint();

public:
	Vector3		m_position		= Vector3::ZERO;
	Quaternion	m_orientation	= Quaternion::IDENTITY;
	float		m_fov			= 45.f;

public:
	static CameraTargetPoint Interpolate( CameraTargetPoint const &fromTP, CameraTargetPoint const &toTP, float byFraction );
};