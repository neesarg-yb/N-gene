#pragma once
#include "CameraState.hpp"
#include "Engine/Math/MathUtil.hpp"

CameraState::CameraState( Vector3 const &velocity, Vector3 const &position, Quaternion const &orientation, float const &fov )
	: m_velocity( velocity )
	, m_position( position )
	, m_orientation( orientation )
	, m_fov( fov )
{

}

CameraState::CameraState( Vector3 const &velocity, Vector3 const &position, Vector3 const &eulerAngleOrientation, float const &fov )
	: m_velocity( velocity )
	, m_position( position )
	, m_orientation( Quaternion::FromEuler(eulerAngleOrientation) )
	, m_fov( fov )
{

}

CameraState::~CameraState()
{

}

Transform CameraState::GetTransform() const
{
	return Transform( m_position, m_orientation, Vector3::ONE_ALL );
}

Matrix44 CameraState::GetTransformMatrix() const
{
	return GetTransform().GetWorldTransformMatrix();
}

CameraState CameraState::Interpolate( CameraState const &a, CameraState const &b, float fraction )
{
	CameraState i;

	i.m_fov				= ::Interpolate( a.m_fov, b.m_fov, fraction );
	i.m_velocity		= ::Interpolate( a.m_velocity, b.m_velocity, fraction );
	i.m_position		= ::Interpolate( a.m_position, b.m_position, fraction );
	i.m_orientation		= Quaternion::Slerp( a.m_orientation, b.m_orientation, fraction );

	return i;
}

