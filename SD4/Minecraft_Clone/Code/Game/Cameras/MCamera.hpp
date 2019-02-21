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

public:
	Vector3		 m_position				= Vector3::ZERO;
	float		 m_yawDegreesAboutZ		= 0.f;
	float		 m_rollDegreesAboutX	= 0.f;
	float		 m_pitchLimit			= 70.f;

private:
	float		 m_pitchDegreesAboutY	= 0.f;

	Matrix44	 m_view;
	Matrix44	 m_projection;
	Matrix44	 m_flip;

	Camera		*m_camera		= nullptr;
	Renderer	&m_renderer;

public:
	Camera*			GetCamera();
	void			RebuildMatrices();
	float			SetPitchDegreesAboutY( float desiredPitch );		// Returns the clamped pitch
	Vector3			GetForwardDirection() const;
	
public:
	inline float	GetPitchDegreesAboutY() const { return m_pitchDegreesAboutY; }
};
