#pragma once
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/CameraSystem/CameraState.hpp"
#include "Game/Camera System/Camera Behaviours/CB_Follow.hpp"

class Terrain;

enum eCameraRelativeInputState
{
	INPUT_LOCKED,				// After the Camera Reorientation has started, when the player wants to keep moving in the same direction
	INPUT_INTERPOLATION,		// When the player initiates a change the current movement direction, usually this would happen once the camera reorientation is finished
	INPUT_UNLOCKED				// When the current movement direction matches with the player input relative to the camera, usually after the Input Interpolation
};

class Player : public GameObject
{
public:
	 Player( Vector3 worldPosition, Terrain const &parentTerrain, Clock *parentClock = nullptr );
	~Player();

public:
	float	const	 m_bodyRadius		= 0.5f;
	Terrain const	*m_terrain			= nullptr;		// The terrain this player is on

	float	const	 m_mass				= 10.f;
	float	const	 m_friction			= 30.f;
	float	const	 m_xzMovementForce	= 1000.f;
	float			 m_defaultMaxSpeed	= 0.f;

	bool m_isPlayerOnTerrainSurface		= false;

private:
	Vector3 m_cameraForward				= Vector3::FRONT;

	eCameraRelativeInputState	m_movementInputState = INPUT_UNLOCKED;
	CameraState					m_cameraStateOnInputLocked;
	Vector2						m_leftStickOnInputLocked;
	Vector2						m_leftStickWhenInterpolating;

	float const m_leftStickReleasedRegionRadiusFraction	= 0.1f;
	float const m_retainInputRegionRadiusFraction		= 0.4f;

	float const	m_inputInterpolationSeconds				= 0.5f;
	Stopwatch	m_inputInterpolationTimer;

	bool		m_debugAutoStopCameraReorientation		= true;

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );
	void InformAboutCameraForward( CameraState const &currentCamState, CB_Follow &followBehavior );

	void ApplyResistantForces();
	void ApplyMovementForces();
	void CheckAndSnapOnTerrainSurface();

	void ApplyForce( float x, float y, float z );
	inline void ApplyForce( Vector3 force ) { ApplyForce( force.x, force.y, force.z ); }

	Vector2 InterpolateInput( Vector2 const &a, Vector2 const &b, float tGetsClamped01 ) const;		// a -> b; t is processed after clamping to [0, 1]

private:
	void UpdateDebugStateFromInput();
	void UpdateCameraForward( CameraState const &currentCamState );
	void LockInputState( CameraState const &camState );
	void StartInputInterpolation( CameraState const &camState );
	void UnlockInputState();

	void DebugRenderLeftStickInput( Vector2 const &screenPosition, float widthSize ) const;
};
