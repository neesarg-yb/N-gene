#pragma once
#include "Engine/Core/GameObject.hpp"
#include "Engine/CameraSystem/CameraMotionController.hpp"

class CMC_ProportionalController : public CameraMotionController
{
public:
	 CMC_ProportionalController( char const *name, CameraManager const *manager );
	~CMC_ProportionalController();

public:
	// Proportional Controller
	bool		m_mpcEnabled				 = true;
	float		m_controllingFactor			 = 15.f;
	float		m_accelerationLimitXZ		 = 500.f;							// Units per second^2

private:
	// Look ahead in player's velocity
	Vector2		m_leadOffsetFromGoal		 = Vector2::ZERO;
	float		m_leadInterpolationFactor	 = 1.f;
	
	bool		m_finalCollisionCheckEnabled = false;

public:
	CameraState MoveCamera( CameraState const &currentState, CameraState const &goalState, float deltaSeconds );

private:
	void	ProcessInput();
	Vector2 UpdateLeadOffset( GameObject const &player, float deltaSeconds );	// Updates the variable m_leadOffsetFromGoal
	void	DebugPrintInformation() const;
	void	FinalCollisionCheck( Vector3 const &currentPosition, Vector3 const &safeDestination, CameraState &goalState, float deltaSeconds );
};
