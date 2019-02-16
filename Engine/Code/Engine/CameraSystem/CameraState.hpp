#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Core/EngineCommon.hpp"

enum eConstraintType : uint
{
	IGNORE_ALL_CONSTRAINT	= 0U,
	APPLY_CORRECTION		= BIT_FLAG(0),
	APPLY_SUGGESTION		= BIT_FLAG(1)
};

class CameraState
{
public:
	 CameraState() { }
	 CameraState( Vector3 const &velocity, Vector3 const &position, Quaternion const &orientation, float const &fov );
	 CameraState( Vector3 const &velocity, Vector3 const &position, Vector3 const &eulerAngleOrientation, float const &fov );
	~CameraState();

public:
	uint			m_constraintType	= (APPLY_CORRECTION | APPLY_SUGGESTION);	// A bit-flag using eConstraintType

	Vector3			m_velocity			= Vector3::ZERO;
	Vector3			m_position			= Vector3::ZERO;
	Quaternion		m_orientation		= Quaternion::IDENTITY;
	float			m_fov				= 45.f;

public:
	Transform	GetTransform() const;
	Matrix44	GetTransformMatrix() const;

public:
	static CameraState Interpolate( CameraState const &fromTP, CameraState const &toTP, float byFraction );
};
