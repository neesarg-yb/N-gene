#pragma once
#include "Engine/Math/Sphere.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Game/MCamera.hpp"
#include "Game/Game States/GameState.hpp"

typedef std::vector< GameObject* >	GameObjectList;
typedef std::vector< Light* >		Lights;

class Scene_ProtoScene3D : public GameState
{
public:
			 Scene_ProtoScene3D( Clock const *parentClock );
	virtual ~Scene_ProtoScene3D();

public:
	// Rendering Specific
	ForwardRenderingPath	*m_renderingPath	= nullptr;
	Scene					*m_scene			= nullptr;
	Lights					 m_lights;
	Vector4					 m_ambientLight		= Vector4( 1.f, 1.f, 1.f, 0.4f );

	// Game Specific 
	GameObjectList			 m_gameObjects[ NUM_ENTITIES ];

	// Obj Models
	Renderable				*m_snowMiku			= nullptr;
	Renderable				*m_spaceship		= nullptr;

private:
	// Game Camera
	MCamera					*m_camera			= nullptr;
	float					 m_flySpeed			= 10.f;
	float					 m_camRotationSpeed = 10.f;

public:
	void JustFinishedTransition();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render( Camera *gameCamera ) const;

	void ProcessInput( float deltaSeconds );

private:
	// Scene Management
	void AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType );
	void AddNewLightToScene( Light *light );
};
