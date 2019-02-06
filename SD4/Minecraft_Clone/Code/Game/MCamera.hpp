#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"

class MCamera
{
public:
	 MCamera( Renderer &activeRenderer );
	~MCamera();

public:
	float		 m_initialFOV	= 45.f;	
	float		 m_cameraNear	= 0.01f;
	float		 m_cameraFar	= 100.f;

private:
	Vector3		 m_position		= Vector3::ZERO;
	Vector3		 m_rotation		= Vector3::ZERO;

	Matrix44	 m_view;
	Matrix44	 m_projection;
	Matrix44	 m_flip;

	Camera		*m_camera		= nullptr;
	Renderer	&m_renderer;

public:
	void	SetPosition( Vector3 const &position );
	void	SetRotation( float x, float y, float z );
	void	SetRotation( Vector3 const &eulerRotDegrees );

public:
	Vector3	GetPosition() const;
	Vector3	GetRotation() const;

	Camera*	GetCamera();
	void	RebuildMatrices();
};
