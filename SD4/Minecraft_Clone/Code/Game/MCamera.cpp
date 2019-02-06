#pragma once
#include "MCamera.hpp"

MCamera::MCamera( Renderer &activeRenderer )
	: m_renderer( activeRenderer )
{
	m_camera = new Camera();

	m_camera->SetColorTarget( m_renderer.GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( m_renderer.GetDefaultDepthTarget() );
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );

	m_camera->EnableShadowMap();
	m_camera->RenderDebugObjects( true );
}

MCamera::~MCamera()
{
	delete m_camera;
	m_camera = nullptr;
}

void MCamera::SetPosition( Vector3 const &position )
{
	m_position = position;
}

void MCamera::SetRotation( float x, float y, float z )
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

void MCamera::SetRotation( Vector3 const &eulerRotDegrees )
{
	SetRotation( eulerRotDegrees.x, eulerRotDegrees.y, eulerRotDegrees.z );
}

Vector3 MCamera::GetPosition() const
{
	return m_position;
}

Vector3 MCamera::GetRotation() const
{
	return m_rotation;
}

Camera* MCamera::GetCamera()
{
	RebuildMatrices();
	return m_camera;
}

void MCamera::RebuildMatrices()
{
	// View Matrix
	m_view.SetIdentity();
	m_view.Append( m_flip );
	m_view.RotateZ3D( m_rotation.z * -1.f );
	m_view.RotateX3D( m_rotation.x * -1.f );
	m_view.RotateY3D( m_rotation.y * -1.f );
	m_view.Translate3D( m_position * -1.f );

	// Projection Matrix
	m_projection = Matrix44::MakePerspective3D( m_initialFOV, g_aspectRatio, m_cameraNear, m_cameraFar );

	// Hammer the matrices on camera!
	m_camera->SetViewMatrixUnsafe( m_view );
	m_camera->SetProjectionMatrixUnsafe( m_projection );
}
