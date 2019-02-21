#pragma once
#include "MCamera.hpp"

MCamera::MCamera( Renderer &activeRenderer )
	: m_renderer( activeRenderer )
{
	m_camera = new Camera();

	m_camera->SetColorTarget( m_renderer.GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( m_renderer.GetDefaultDepthTarget() );

	m_flip.SetIColumn( Vector3( 0.f, 0.f, 1.f) );
	m_flip.SetJColumn( Vector3(-1.f, 0.f, 0.f) );
	m_flip.SetKColumn( Vector3( 0.f, 1.f, 0.f) );
}

MCamera::~MCamera()
{
	delete m_camera;
	m_camera = nullptr;
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
	m_view.RotateX3D( -m_rollDegreesAboutX  );
	m_view.RotateY3D( -m_pitchDegreesAboutY );
	m_view.RotateZ3D( -m_yawDegreesAboutZ );
	m_view.Translate3D( -m_position );

	// Projection Matrix
	m_projection = Matrix44::MakePerspective3D( m_initialFOV, g_aspectRatio, m_cameraNear, m_cameraFar );

	// Hammer the matrices on camera!
	m_camera->SetViewMatrixUnsafe( m_view );
	m_camera->SetProjectionMatrixUnsafe( m_projection );
}

float MCamera::SetPitchDegreesAboutY( float desiredPitch )
{
	m_pitchDegreesAboutY = desiredPitch;
	m_pitchDegreesAboutY = ClampFloat( m_pitchDegreesAboutY, -m_pitchLimit, +m_pitchLimit );

	return m_pitchDegreesAboutY;
}

Vector3 MCamera::GetForwardDirection() const
{
	Matrix44 rotationMatrix;

	rotationMatrix.RotateZ3D( m_yawDegreesAboutZ );
	rotationMatrix.RotateY3D( m_pitchDegreesAboutY );
	rotationMatrix.RotateX3D( m_rollDegreesAboutX );

	return rotationMatrix.Multiply( Vector3( 1.f, 0.f, 0.f ), 0.f );
}
