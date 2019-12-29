#pragma once
#include "Engine/CameraSystem/CameraBehaviour.hpp"

class CB_ZoomCamera : public CameraBehaviour
{
public:
	 CB_ZoomCamera( Vector3 const &refPos, float fov, std::string const behaviorName, CameraManager const *manager );
	~CB_ZoomCamera();

private:
	float		m_refRotYawWs			= 0.f;				// Degrees
	float		m_refRotPitchWs			= 0.f;				// Degrees

	float		m_reticleYawDegreesWs	= 0.f;				// Because of the reticle offset
	float		m_reticlePitchDegreesWs	= 0.f;
	float		m_camYawExtraRot		= 0.f;				// Degrees
	float		m_camPitchExtraRot		= 0.f;				// Degrees

	float const	m_minPitchDegrees		= -90.f;
	float const m_maxPitchDegrees		=  90.f;

	IntVector2	m_reticleOffset			= IntVector2::ZERO;	// In screen space

	Transform	m_referenceTranform;
	Vector3		m_cameraOffset;
	float		m_fov;

	CameraState	m_cameraState;

public:
	float		m_mouseSensitivity = 0.5f;					// Multiplier to delta mouse pos.

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
	void		SetCameraYawExtraRotation( float yawDegreesExtra );
	void		SetCameraPitchExtraRotation( float pitchDegreesExtra );
	void		SetReticleOffset( IntVector2 reticleOffsetSs );

	void		LookAtTargetPosition( Vector3 const &targetWs );

	Transform	GetReferenceTransform() const { return m_referenceTranform; }
	float		GetCameraYawExtraRotation() const { return m_camYawExtraRot; }
};
