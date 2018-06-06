#pragma once
#include "Camera.hpp"

void Camera::SetColorTarget( Texture *color_target )
{
	m_output_framebuffer.SetColorTarget( color_target );
}

void Camera::SetDepthStencilTarget( Texture *depth_target )
{
	m_output_framebuffer.SetDepthStencilTarget( depth_target );
}

void Camera::Finalize()
{
	m_output_framebuffer.Finalize();
}

unsigned int Camera::GetFrameBufferHandle() const
{
	return m_output_framebuffer.GetHandle();
}

void Camera::LookAt( Vector3 position, Vector3 target, Vector3 up /* = Vector3::UP */ )
{

	Vector3 K = (target - position).GetNormalized();
	Vector3 I = Vector3::CrossProduct(up, K).GetNormalized();
	Vector3 J = Vector3::CrossProduct(K, I);
	Vector3 T = position;

	float array[16] = { I.x, I.y, I.z, 0.f, J.x, J.y, J.z, 0.f, K.x, K.y, K.z, 0.f, T.x, T.y, T.z, 1.f };
	m_camera_matrix = Matrix44( array );
	m_view_matrix	= m_camera_matrix.GetOrthonormalInverse();
}

void Camera::SetProjectionOrtho( float size, float screen_near, float screen_far )
{
	m_size		  = size;
	m_screen_near = screen_near;
	m_screen_far  = screen_far;

	m_proj_matrix = Matrix44::MakeOrtho3D( size * m_aspectRatio, size, screen_near, screen_far );
}

void Camera::IncrementCameraSizeBy( float sizeIncrement )
{
	if( m_size + sizeIncrement <= 0.f )
		return;
	
	m_size += sizeIncrement;
	SetProjectionOrtho( m_size, m_screen_near, m_screen_far );
}

void Camera::SetPerspectiveCameraProjectionMatrix( float fovDegrees, float aspectRatio, float nearZ, float farZ )
{
	m_proj_matrix = Matrix44::MakePerspective3D( fovDegrees, aspectRatio, nearZ, farZ );
}

void Camera::SetCameraPositionTo( Vector3 const &newPosition )
{
	m_camera_matrix.Tx	= newPosition.x;
	m_camera_matrix.Ty	= newPosition.y;
	m_camera_matrix.Tz	= newPosition.z;

	// Modify the View Matrix, which is inverse of the Camera Matrix
	m_view_matrix = m_camera_matrix.GetOrthonormalInverse();
}

void Camera::SetCameraEulerRotationTo( Vector3 const &newEulerRotation )
{
	Matrix44 rotationMatrix;
	rotationMatrix.RotateDegrees3D( newEulerRotation );
	
	m_camera_matrix.Ix = rotationMatrix.Ix;
	m_camera_matrix.Iy = rotationMatrix.Iy;
	m_camera_matrix.Iz = rotationMatrix.Iz;

	m_camera_matrix.Jx = rotationMatrix.Jx;
	m_camera_matrix.Jy = rotationMatrix.Jy;
	m_camera_matrix.Jz = rotationMatrix.Jz;

	m_camera_matrix.Kx = rotationMatrix.Kx;
	m_camera_matrix.Ky = rotationMatrix.Ky;
	m_camera_matrix.Kz = rotationMatrix.Kz;

	// Modify the View Matrix, which is inverse of the Camera Matrix
	m_view_matrix = m_camera_matrix.GetOrthonormalInverse();
}

void Camera::MoveCameraPositionBy( Vector3 const &localTranslation )
{
	Matrix44 translationMatrix;
	translationMatrix.Translate3D( localTranslation );

	m_camera_matrix.Append( translationMatrix );

	// Modify the View Matrix, which is inverse of the Camera Matrix
	m_view_matrix = m_camera_matrix.GetOrthonormalInverse();
}

void Camera::RotateCameraBy( Vector3 const &localRotation )
{
	Matrix44 rotationMatrix;
	rotationMatrix.RotateDegrees3D( localRotation );

	m_camera_matrix.Append( rotationMatrix );

	// Modify the View Matrix, which is inverse of the Camera Matrix
	m_view_matrix = m_camera_matrix.GetOrthonormalInverse();
}

Vector3 Camera::GetForwardVector() const 
{
	return m_view_matrix.GetKColumn();
}

Vector3 Camera::GetEulerRotation() const
{
	return m_camera_matrix.GetEulerRotation();
}