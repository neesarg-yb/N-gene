#pragma once
#include "Engine/Math/Sphere.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Game/World/Player.hpp"
#include "Game/World/Terrain.hpp"
#include "Game/Abstract Classes/GameState.hpp"
#include "Game/Camera System/DebugCamera.hpp"
#include "Game/Camera System/Camera Behaviours/CB_FreeLook.hpp"
#include "Game/Camera System/Motion Controllers/CMC_ProportionalController.hpp"

typedef std::vector< GameObject* >	GameObjectList;
typedef std::vector< Light* >		Lights;

constexpr int TOTAL_BUILDINGS = 10;

class Scene_ProtoScene3D : public GameState
{
public:
			 Scene_ProtoScene3D( Clock const *parentClock );
	virtual ~Scene_ProtoScene3D();

public:
	// Rendering Specific
	ForwardRenderingPath	*m_renderingPath	= nullptr;
	Scene					*m_scene			= nullptr;
	Camera					*m_camera			= nullptr;
	DebugCamera				*m_debugCamera		= nullptr;
	Lights					 m_lights;
	Vector4					 m_ambientLight		= Vector4( 1.f, 1.f, 1.f, 0.6f );

	// Camera Behaviours
	CB_FreeLook					*m_debugFreelook			= nullptr;
	CMC_ProportionalController	*m_proportionalController	= nullptr;

	// Game Specific
	GameObjectList			 m_gameObjects[ NUM_ENTITIES ];
	Terrain					*m_terrain			= nullptr;
	Player					*m_player			= nullptr;

private:
	// Camera
	CameraManager			*m_cameraManager			= nullptr;
	float const				 m_initialFOV				= 45.f;			// Degrees
	float const				 m_changeFOVSpeed			= 10.f;			// Degrees per seconds
	float const				 m_cameraNear				= 0.001f;
	float const				 m_cameraFar				= 1000.f;
	bool					 m_debugCameraFullOverlay	= false;
	bool					 m_debugCameraEnabled		= false;

public:
	void JustFinishedTransition();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render( Camera *gameCamera ) const;

	RaycastResult	Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance );
	Vector3			SphereCollision( Sphere const &sphere );

private:
	// Scene Management
	void AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType );
	void AddNewLightToScene( Light *light );

	// Clock
	void ChangeClocksTimeScale();		// Should happen after Scene::Update()
	void PauseUnpauseClock();			// Should happen after Scene::Update()

	// Debug Overlay
	void ChangeDebugCameraSettings();
	void DebugRenderHotkeys();
};
