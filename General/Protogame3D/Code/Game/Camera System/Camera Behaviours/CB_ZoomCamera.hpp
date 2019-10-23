#pragma once
#include "Engine/CameraSystem/CameraBehaviour.hpp"

class CB_ZoomCamera : public CameraBehaviour
{
public:
	 CB_ZoomCamera( Vector3 const &refPos, float fov, std::string const behaviorName, CameraManager const *manager );
	~CB_ZoomCamera();

private:
	Transform	m_referenceTranform;
	Vector3		m_cameraOffset;
	float		m_fov;

public:
	float		m_rotSpeed = 40.f;	// Degrees per seconds

public:
	void		PreUpdate() override;
	void		PostUpdate() override;
	CameraState	Update( float deltaSeconds, CameraState const &currentState );

private:
	void		UpdateReferenceRotation( float deltaSeconds );

public:
	void		SetReferencePosition( Vector3 const &refPosWs );
	void		SetCameraOffsetFromReference( Vector3 const &camOffset );
	void		LookAtTargetPosition( Vector3 const &targetWs );
};
