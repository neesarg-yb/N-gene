#pragma once
#include "Engine/CameraSystem/CameraBehaviour.hpp"

class CC_HandoverToFollow;

class CB_ShoulderView : public CameraBehaviour
{
	friend CC_HandoverToFollow;

public:
	 CB_ShoulderView( float heightRelativeToAnchorPos, float radiusFromAnchor, float localCameraOffsetX, float minPitchOffset, float maxPitchOffset, char const *name, CameraManager *manager );
	~CB_ShoulderView();

private:
	float		m_rotationAroundAnchor		= 0.f;		// Degrees
	float		m_localPitchOffset			= 0.f;		// Degrees
	float		m_radiusFromAnchor			= 0.5f;		// Positive. Towards back of the player's head
	float		m_localCameraOffsetX		= 0.2f;		// Positive is towards Right
	float		m_heightFromAnchorPosition	= 0.f;		// On world Y Axis, from anchor's XZ position

	float		m_rotationChangeSpeed		= 50.f;		// Degrees per seconds
	FloatRange	m_cameraPitchOffsetLimits	= FloatRange( -16.f, 16.f );

public:
	void		PreUpdate();
	void		PostUpdate();
	CameraState	Update( float deltaSeconds, CameraState const &currentState );

	void		SetupForIncomingHandover( float rotationAroundAnchor, Vector3 const &currentCameraPos );

private:
	void		ProcessInput( float deltaSeconds );
	void		DebugPrintInputInformation() const;

	Vector3		GetCameraPosition( bool onRightShoulder, float rotation ) const;
	bool		IsCurrentCameraPositionNearToRightShoulder( float rotationAroundAnchor, Vector3 const &cameraPos );		// If near left shoulder, it returns false
};
