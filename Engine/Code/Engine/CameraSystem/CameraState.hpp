#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Transform.hpp"

class CameraState
{
public:
	 CameraState() { }
	 CameraState( Vector3 const &velocity, Vector3 const &position, Quaternion const &orientation, float const &fov );
	 CameraState( Vector3 const &velocity, Vector3 const &position, Vector3 const &eulerAngleOrientation, float const &fov );
	~CameraState();

public:
	Vector3		m_velocity		= Vector3::ZERO;
	Vector3		m_position		= Vector3::ZERO;
	Quaternion	m_orientation	= Quaternion::IDENTITY;
	float		m_fov			= 45.f;

public:
	Transform	GetTransform() const;
	Matrix44	GetTransformMatrix() const;

	Vector3		GetWorldCoordFromScreen( IntVector2 screenCoord, float screenNearZ, float screenFarZ ) const;

public:
	static CameraState Interpolate( CameraState const &fromTP, CameraState const &toTP, float byFraction );
};
