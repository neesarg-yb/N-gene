#pragma once
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Game/Game States/GameState.hpp"

class Scene_QuaternionsTest : public GameState
{
public:
	 Scene_QuaternionsTest( Clock const *parentClock );
	~Scene_QuaternionsTest();

public:
	void JustFinishedTransition();

	void BeginFrame();
	void EndFrame();

	void Update();
	void Render( Camera *gameCamera ) const;

private:
	// Rendering Specific
	Camera*						m_camera					= nullptr;
	Vector4						m_ambientLight				= Vector4( 1.f, 1.f, 1.f, 0.5f );

	// Rotation
	float	const				m_rotationSpeed				= 40.f;								// Degrees per second
	Vector3						m_currentEulerRotation		= Vector3::ZERO;
	Vector3						m_targetSlerpEulerRotation	= Vector3::ZERO;
	Vector3	const				m_eulerBasisWorldPos		= Vector3( -10.f, 0.f, 0.f );
	Vector3	const				m_quaternionBasisWorldPos	= Vector3( +10.f, 0.f, 0.f );

public:
	std::string					m_levelName;

private:
	// Local
	double						m_timeSinceStartOfTheBattle	= 0;
	double						m_timeWhenSlerpStarted		= 0;

	// Slerp
	float						m_t							= 0.f;
	float const					m_slerpDurationSeconds		= 3.f;
	bool						m_performSlerpOperation		= false;

private:
	void QuaternionsTestCode() const;
	void RenderMeshUsingEuler( Vector3 const &position, Vector3 const &rotationInDegrees ) const;
	void RenderMeshUsingQuaternion( Vector3 const &position, Vector3 const &rotationInDegrees ) const;
	void UpdateEulerRotationAccordingToInput( float deltaSeconds );

	void StartTheSlerp();
	void StoreTheTargetEulerRotationForSlerp();
};