#pragma once
#include <vector>
#include "Engine/Core/GameObject.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/OrbitCamera.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Game/Abstract Classes/GameState.hpp"
#include "Game/World/Terrain.hpp"
#include "Game/World/Player.hpp"

typedef std::vector< GameObject* >	GameObjectList;
typedef std::vector< Light* >		Lights;

class Scene_DegreesOfFreedom : public GameState
{
public:
			 Scene_DegreesOfFreedom( Clock const *parentClock );
	virtual ~Scene_DegreesOfFreedom();

public:
	// Rendering Specific
	ForwardRenderingPath	*m_renderingPath	= nullptr;
	Scene					*m_scene			= nullptr;
	Camera					*m_camera			= nullptr;
	Lights					 m_lights;
	Vector4					 m_ambientLight		= Vector4( 1.f, 1.f, 1.f, 0.6f );

	// Game Specific
	GameObjectList			 m_gameObjects;
	Terrain					*m_terrain			= nullptr;
	Player					*m_player			= nullptr;

private:
	// Camera
	CameraManager			*m_cameraManager	= nullptr;
	float const				 m_initialFOV		= 45.f;			// Degrees
	float const				 m_changeFOVSpeed	= 10.f;			// Degrees per seconds
	float const				 m_cameraNear		= 1.f;
	float const				 m_cameraFar		= 1000.f;

public:
	void JustFinishedTransition();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render( Camera *gameCamera ) const;

private:
	void AddNewGameObjectToScene( GameObject *go );
	void AddNewLightToScene( Light *light );

	void ChangeCameraBehaviour();
};