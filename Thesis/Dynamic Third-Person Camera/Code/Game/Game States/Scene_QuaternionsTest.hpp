#pragma once
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Game/Abstract Classes/GameState.hpp"
#include "Game/Abstract Classes/GameObject.hpp"

typedef std::vector< GameObject* > GameObjectList;

class Scene_QuaternionsTest : public GameState
{
public:
	 Scene_QuaternionsTest();
	~Scene_QuaternionsTest();

public:
	void BeginFrame();
	void EndFrame();

	void Update( float deltaSeconds );
	void Render( Camera *gameCamera ) const;

private:
	// Rendering Specific
	Camera*						m_camera					= nullptr;
	Vector4						m_ambientLight				= Vector4( 1.f, 1.f, 1.f, 0.5f );

	// Rotation
	float	const				m_rotationSpeed				= 40.f;								// Degrees per second
	Vector3						m_currentEulerRotation		= Vector3::ZERO;
	Vector3	const				m_eulerBasisWorldPos		= Vector3( -10.f, 0.f, 0.f );
	Vector3	const				m_quaternionBasisWorldPos	= Vector3( +10.f, 0.f, 0.f );

public:
	std::string					m_levelName;

private:
	// Local
	double						m_timeSinceStartOfTheBattle	= 0;

private:
	void QuaternionsTestCode() const;
	void RenderMeshUsingEuler( Vector3 const &position, Vector3 const &rotationInDegrees ) const;
	void UpdateEulerRotationAccordingToInput( float deltaSeconds );
};