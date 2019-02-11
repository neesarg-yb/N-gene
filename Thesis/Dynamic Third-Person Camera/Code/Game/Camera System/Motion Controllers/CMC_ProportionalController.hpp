#pragma once
#include "Engine/CameraSystem/CameraMotionController.hpp"

class CMC_ProportionalController : public CameraMotionController
{
public:
	 CMC_ProportionalController( char const *name, CameraManager const *manager );
	~CMC_ProportionalController();

public:
	// Proportional Controller
	bool		m_mpcEnabled				= true;
	float		m_controllingFactor			= 12.f;
	float		m_accelerationLimitXZ		= 80.f;							// Units per second^2
	float		m_leadFactor				= 1.0f;

public:
	CameraState MoveCamera( CameraState const &currentState, CameraState const &goalState, float deltaSeconds );

private:
	void ProcessInput();
	void DebugPrintInformation() const;
};
