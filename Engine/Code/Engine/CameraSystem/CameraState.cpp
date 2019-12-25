#pragma once
#include "CameraState.hpp"
#include "Engine/Core/Window.hpp"
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

Vector3 CameraState::GetWorldCoordFromScreen( IntVector2 screenCoord, float screenNearZ, float screenFarZ ) const
{
	// Screen to NDC
	Vector3 ndcPos;
	{
		float const ndcZ = -1.f;
		float const clientWidth  = (float) Window::GetInstance()->GetWidth();
		float const clientHeight = (float) Window::GetInstance()->GetHeight();

		ndcPos.x	= RangeMapFloat( (float)screenCoord.x, 0.f, clientWidth, -1.f, 1.f );
		ndcPos.y	= RangeMapFloat( (float)screenCoord.y, 0.f, clientHeight, 1.f, -1.f );
		ndcPos.z	= ClampFloat( ndcZ, -1.f, 1.f );
	}

	// NDC to View
	Vector4	 viewPos;
	{
		float	 const aspectRatio	= Window::GetInstance()->GetAspectRatio();
		Matrix44 const projMatrix	= Matrix44::MakePerspective3D( m_fov, aspectRatio, screenNearZ, screenFarZ );
		
		Matrix44 invProjMatrix;
		bool const valid = projMatrix.GetInverse( invProjMatrix );
		GUARANTEE_RECOVERABLE( valid, "Warning: Couln't inverse the Matrix!!" );

		viewPos	= invProjMatrix.Multiply( Vector4( ndcPos, 1.f ) );
	}

	// View to World
	Vector4 worldPos4;
	{
		Transform const cameraTransform	= Transform( m_position, m_orientation, Vector3::ONE_ALL );
		Matrix44  const viewMatrix		= cameraTransform.GetWorldTransformMatrix().GetOrthonormalInverse();
		
		Matrix44 invViewMatrix;
		bool const valid = viewMatrix.GetInverse( invViewMatrix );
		GUARANTEE_RECOVERABLE( valid, "Warning: Couln't inverse the Matrix!!" );

		worldPos4 = invViewMatrix.Multiply( viewPos );
	}

	Vector3 const worldPos = Vector3( worldPos4.x, worldPos4.y, worldPos4.z ) / worldPos4.w;

	return worldPos;
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

