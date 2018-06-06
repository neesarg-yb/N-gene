#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Transform
{
private:
	mutable bool		m_isDirty			=	false;
	mutable Matrix44	m_transformMatrix;	// Initializes as Identity Matrix
	Vector3				m_position			= Vector3::ZERO;
	Vector3				m_rotation			= Vector3::ZERO;				// TODO: Change it to ( Roll-Pitch-Yaw ); Euler should be ZXY.
	Vector3				m_scale				= Vector3::ONE_ALL;

public:
	Transform() { };
	Transform( Vector3 const &position, Vector3 const &rotation, Vector3 const &scale );

public:
	Vector3		GetPosition() const;
	Vector3		GetRotation() const;
	Vector3		GetScale   () const;

	Matrix44	GetTransformMatrix() const;

	void		SetPosition( Vector3 const &position );
	void		SetRotation( Vector3 const &rotation );			// rotation = Vec3(Pitch, Yaw, Roll); but the order the rotation is applied is: Roll_Z -> Pitch_X -> Yaw_Y
	void		SetScale   ( Vector3 const &scale	 );

private:
	void		RecalculateTheMatrix() const;
};