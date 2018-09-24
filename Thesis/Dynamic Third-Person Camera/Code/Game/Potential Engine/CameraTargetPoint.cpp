#pragma once
#include "CameraTargetPoint.hpp"
#include "Engine/Math/MathUtil.hpp"

CameraTargetPoint::CameraTargetPoint( Vector3 const &position, Quaternion const &orientation, float const &fov )
	: m_position( position )
	, m_orientation( orientation )
	, m_fov( fov )
{

}

CameraTargetPoint::CameraTargetPoint( Vector3 const &position, Vector3 const &eulerAngleOrientation, float const &fov )
	: m_position( position )
	, m_orientation( Quaternion::FromEuler(eulerAngleOrientation) )
	, m_fov( fov )
{

}

CameraTargetPoint::~CameraTargetPoint()
{

}

CameraTargetPoint CameraTargetPoint::Interpolate( CameraTargetPoint const &a, CameraTargetPoint const &b, float fraction )
{
	CameraTargetPoint i;

	i.m_fov				= ::Interpolate( a.m_fov, b.m_fov, fraction );
	i.m_position		= ::Interpolate( a.m_position, b.m_position, fraction );
	i.m_orientation		= Quaternion::Slerp( a.m_orientation, b.m_orientation, fraction );

	return i;
}

