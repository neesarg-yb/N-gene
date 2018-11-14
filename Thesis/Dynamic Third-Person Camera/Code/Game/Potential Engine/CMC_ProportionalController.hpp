#pragma once
#include "Game/Potential Engine/CameraMotionController.hpp"

class CMC_ProportionalController : public CameraMotionController
{
public:
	 CMC_ProportionalController( char const *name, CameraManager const *manager );
	~CMC_ProportionalController();

public:
	// Proportional Controller
	bool		m_mpcEnabled				= true;
	float		m_controllingFactor			= 8.5f;
	float		m_accelerationLimitXZ		= 35.f;							// Units per second^2
	float		m_leadFactor				= 1.f;

	// Look At Anchor
	bool		m_lookAtAnchor				= false;

public:
	void		LookAtAnchorAfterMoved( bool enabled );
	CameraState MoveCamera( CameraState const &currentState, CameraState const &goalState, float deltaSeconds );

private:
	void ProcessInput();
	void DebugPrintInformation() const;
};
