#pragma once
#include "Engine/CameraSystem/CameraBehaviour.hpp"

class CB_ZoomCamera : public CameraBehaviour
{
public:
	 CB_ZoomCamera( Vector3 const &refPos, float fov, std::string const behaviorName, CameraManager const *manager );
	~CB_ZoomCamera();

private:
	float		m_refRotYaw			= 0.f;
	float		m_refRotPitch		= 0.f;

	float const	m_minPitchDegrees	= -90.f;
	float const m_maxPitchDegrees	=  90.f;

	Transform	m_referenceTranform;
	Vector3		m_cameraOffset;
	float		m_fov;

public:
	float		m_mouseSensitivity = 0.5f;	// Multiplier to delta mouse pos.

public:
	void		PreUpdate() override;
	void		PostUpdate() override;
	CameraState	Update( float deltaSeconds, CameraState const &currentState );

private:
	void		UpdateReferenceRotation();
	void		GetExtraRotationForReticleOffset( Vector2 const &reticlePos, Vector2 const screenDimensions, float &yawDegrees_out, float &pitchDegrees_out );

public:
	void		SetReferencePosition( Vector3 const &refPosWs );
	void		SetCameraOffsetFromReference( Vector3 const &camOffset );
	void		LookAtTargetPosition( Vector3 const &targetWs, Vector2 const &reticlePos, Vector2 const &screenDimensions );

	Transform	GetReferenceTransform() const { return m_referenceTranform; }
};
