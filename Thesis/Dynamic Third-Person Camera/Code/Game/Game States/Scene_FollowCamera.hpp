#pragma once
#include "Engine/Math/Sphere.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Game/Potential Engine/GameObject.hpp"
#include "Game/Potential Engine/CameraManager.hpp"
#include "Game/Abstract Classes/GameState.hpp"
#include "Game/World/Player.hpp"
#include "Game/World/Terrain.hpp"

typedef std::vector< GameObject* >	GameObjectList;
typedef std::vector< Light* >		Lights;

class Scene_FollowCamera : public GameState
{
public:
			 Scene_FollowCamera( Clock const *parentClock );
	virtual ~Scene_FollowCamera();

public:
	// Rendering Specific
	ForwardRenderingPath	*m_renderingPath	= nullptr;
	Scene					*m_scene			= nullptr;
	Camera					*m_camera			= nullptr;
	Lights					 m_lights;
	Vector4					 m_ambientLight		= Vector4( 1.f, 1.f, 1.f, 0.6f );

	// Game Specific
	GameObjectList			 m_gameObjects[ NUM_ENTITIES ];
	Terrain					*m_terrain			= nullptr;
	Player					*m_player			= nullptr;

private:
	// Camera
	CameraManager			*m_cameraManager	= nullptr;
	float const				 m_initialFOV		= 45.f;			// Degrees
	float const				 m_changeFOVSpeed	= 10.f;			// Degrees per seconds
	float const				 m_cameraNear		= 0.001f;
	float const				 m_cameraFar		= 1000.f;
	bool					 m_constrainsActive = true;

public:
	void JustFinishedTransition();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render( Camera *gameCamera ) const;

	RaycastResult	Raycast( Vector3 const &startPosition, Vector3 const &direction, float maxDistance );
	Vector3			SphereCollision( Sphere const &sphere );

private:
	void AddNewGameObjectToScene( GameObject *go, WorldEntityTypes entityType );
	void AddNewLightToScene( Light *light );

	void ChangeCameraBehaviour();
	void EnableDisableCameraConstrains();

	void DebugRenderTerrainNormalRaycast() const;
};
