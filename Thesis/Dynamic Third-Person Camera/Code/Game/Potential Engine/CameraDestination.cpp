#pragma once
#include "CameraDestination.hpp"
#include "Engine/Math/MathUtil.hpp"

CameraDestination::CameraDestination( Vector3 const &position, Quaternion const &orientation, float const &fov )
	: m_position( position )
	, m_orientation( orientation )
	, m_fov( fov )
{

}

CameraDestination::CameraDestination( Vector3 const &position, Vector3 const &eulerAngleOrientation, float const &fov )
	: m_position( position )
	, m_orientation( Quaternion::FromEuler(eulerAngleOrientation) )
	, m_fov( fov )
{

}

CameraDestination::~CameraDestination()
{

}

CameraDestination CameraDestination::Interpolate( CameraDestination const &a, CameraDestination const &b, float fraction )
{
	CameraDestination i;

	i.m_fov				= ::Interpolate( a.m_fov, b.m_fov, fraction );
	i.m_position		= ::Interpolate( a.m_position, b.m_position, fraction );
	i.m_orientation		= Quaternion::Slerp( a.m_orientation, b.m_orientation, fraction );

	return i;
}

