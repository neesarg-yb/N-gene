#pragma once
#include "Engine/Math/Sphere.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Game/Game States/GameState.hpp"
#include "Game/Camera System/DebugCamera.hpp"
#include "Game/Camera System/Camera Behaviours/CB_FreeLook.hpp"
#include "Game/Camera System/Motion Controllers/CMC_ProportionalController.hpp"

typedef std::vector< GameObject* >	GameObjectList;
typedef std::vector< Light* >		Lights;

class CB_ZoomCamera;

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
	// Camera
	Camera					*m_camera					= nullptr;
	CameraManager			*m_cameraManager			= nullptr;
	float const				 m_initialFOV				= 50.f;			// Degrees
	float const				 m_cameraNear				= 0.01f;
	float const				 m_cameraFar				= 100.f;

	// Debug Camera
	DebugCamera				*m_debugCamera				= nullptr;
	CB_FreeLook				*m_debugCBFreeLook			= nullptr;

	Renderable				*m_testCubeRenderable		= nullptr;

	// Zoom Camera
	bool					 m_zoomCameraActive			= false;
	CB_ZoomCamera			*m_zoomCameraBehavior		= nullptr;

	// Target Point
	bool					 m_newTargetJustSpawnned	= false;
	Vector3					 m_targetPointWs			= Vector3( -5.f, 0.f, 10.f );

public:
	void JustFinishedTransition();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render( Camera *gameCamera ) const;

private:
	void CheckSwitchCameraBehavior();

	// Zoom Camera
	void SpawnTargetOnSpaceBar();
	void RenderTarget() const;
	void DebugRenderZoomCamera() const;

	// Scene Management
	void AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType );
	void AddNewLightToScene( Light *light );
};
