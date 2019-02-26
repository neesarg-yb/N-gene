#pragma once
#include "Engine/Core/GameObject.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Game/World/Terrain.hpp"
#include "Game/Game States/GameState.hpp"

typedef std::vector< GameObject* >	GameObjectList;
typedef std::vector< Light* >		Lights;

class Scene_AverageRotation : public GameState
{
public:
	 Scene_AverageRotation( Clock const *parentClock );
	~Scene_AverageRotation();

private:
	// Rendering Specific
	ForwardRenderingPath	*m_renderingPath	= nullptr;
	Scene					*m_scene			= nullptr;
	Camera					*m_camera			= nullptr;
	Lights					 m_lights;
	Vector4					 m_ambientLight		= Vector4( 1.f, 1.f, 1.f, 0.6f );

	// Game Specific
	GameObjectList			 m_gameObjects[ NUM_ENTITIES ];
	Terrain					*m_terrain			= nullptr;

	// Camera
	CameraManager			*m_cameraManager			= nullptr;
	float const				 m_initialFOV				= 45.f;			// Degrees
	float const				 m_cameraNear				= 0.001f;
	float const				 m_cameraFar				= 1000.f;

public:
	void JustFinishedTransition();
	void BeginFrame();
	void EndFrame();

	void Update();
	void Render( Camera *gameCamera ) const;

private:
	// Scene Management
	void AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType );
	void AddNewLightToScene( Light *light );
};
