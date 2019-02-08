#pragma once
#include "Engine/Math/Sphere.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Game/World/Cube.hpp"
#include "Game/Cameras/MCamera.hpp"
#include "Game/Game States/GameState.hpp"

class MinecraftWorld : public GameState
{
public:
			 MinecraftWorld( Clock const *parentClock, char const *sceneName );
	virtual ~MinecraftWorld();

public:
	// Rendering Specific
	Vector4					 m_ambientLight		= Vector4( 1.f, 1.f, 1.f, 0.7f );

	// Game Specific 
	Cube					 m_testCube			= Cube( Vector3::ONE_ALL );
	Material				*m_cubeMaterial		= nullptr;

private:
	// Game Camera
	MCamera					*m_camera			= nullptr;
	float					 m_flySpeed			= 5.f;
	float					 m_camRotationSpeed = 0.2f;

public:
	void JustFinishedTransition();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render( Camera *gameCamera ) const;

	void ProcessInput( float deltaSeconds );

private:
	void RenderBasis( float length ) const;
};
