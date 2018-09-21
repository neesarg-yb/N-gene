#pragma once
#include <vector>
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Abstract Classes/GameState.hpp"
#include "Game/World/GameObject.hpp"
#include "Game/World/Terrain.hpp"

typedef std::vector< GameObject* >	GameObjectList;
typedef std::vector< Light* >		Lights;

class Scene_DegreesOfFreedom : public GameState
{
public:
			 Scene_DegreesOfFreedom();
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

public:
	void BeginFrame();
	void EndFrame();
	void Update( float deltaSeconds );
	void Render( Camera *gameCamera ) const;

private:
	void AddNewGameObjectToScene( GameObject *go );
	void AddNewLightToScene( Light *light );
};