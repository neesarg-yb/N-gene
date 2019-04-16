#pragma once
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"

class World;
class Player;

class MCamera
{
public:
	 MCamera( Renderer &activeRenderer, Clock* parentClock, Player const *anchor, World const *inTheWorld );
	~MCamera();

public:
	// Frustum
	float		 m_initialFOV			= 45.f;	
	float		 m_cameraNear			= 0.01f;
	float		 m_cameraFar			= 100.f;

	// Transform
	Vector3		 m_position				= Vector3::ZERO;
	float		 m_yawDegreesAboutZ		= 0.f;
	float		 m_rollDegreesAboutX	= 0.f;
	float		 m_pitchLimit			= 70.f;

	// Movement
	float		 m_cameraFlySpeed		= 8.f;
	float		 m_camRotationSpeed		= 0.2f;

private:
	Clock		 m_clock;
	float		 m_pitchDegreesAboutY	= 0.f;

	// Matrices
	Matrix44	 m_view;
	Matrix44	 m_projection;
	Matrix44	 m_flip;

	// Base
	Camera		*m_camera				= nullptr;
	Renderer	&m_renderer;

public:
	// Camera Behavior
	World  const	*m_world					= nullptr;
	Player const	*m_anchorPlayer				= nullptr;
	eCameraMode		 m_cameraMode				= CAMERA_DETATCHED;
	bool			 m_inputControlsCamera		= false;
	float			 m_overTheShoulderRadius	= 8.f;
	float			 m_moveFasterScale			= 4.5;			// When pressed SHIFT, while in Detached Mode

public:
	void	Update();

private:
	void	Update_CameraDetatched( float deltaSeconds );
	void	Update_Camera1stPerson( float deltaSeconds );
	void	Update_CameraOverTheShoulder( float deltaSeconds );
	void	Update_CameraFixedAngle( float deltaSeconds );

public:
	Camera*	GetCamera();
	void	RebuildMatrices();
	float	SetPitchDegreesAboutY( float desiredPitch );		// Returns the clamped pitch
	Vector3	GetForwardDirection() const;
	
public:
	inline float GetPitchDegreesAboutY() const { return m_pitchDegreesAboutY; }
};
