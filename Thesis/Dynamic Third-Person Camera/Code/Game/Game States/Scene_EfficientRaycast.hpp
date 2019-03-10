#pragma once
#include "Engine/Core/GameObject.hpp"
#include "Engine/Core/RaycastResult.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Game/World/Terrain.hpp"
#include "Game/Game States/GameState.hpp"

typedef std::vector< GameObject* >	GameObjectList;
typedef std::vector< Light* >		Lights;

class Scene_EffecientRaycast : public GameState
{
public:
	 Scene_EffecientRaycast( Clock const *parentClock );
	~Scene_EffecientRaycast();

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
	int const				 m_maxBuildings		= 1;

	// Camera
	CameraManager			*m_cameraManager			= nullptr;
	float const				 m_initialFOV				= 45.f;			// Degrees
	float const				 m_cameraNear				= 0.001f;
	float const				 m_cameraFar				= 1000.f;

	// Raycast
	float					 m_rayMaxLength				= 10.f;
	Vector3					 m_rayStartPos				= Vector3::ZERO;
	RaycastResult			 m_raycastResult			= RaycastResult( Vector3::ZERO );

public:
	void JustFinishedTransition();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render( Camera *gameCamera ) const;

	RaycastResult Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance ) const;

private:
	// Scene Management
	void AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType );
	void AddNewLightToScene( Light *light );

	void PerformDebugTestRaycasts( Matrix44 const &cameraTransformMat );
};
