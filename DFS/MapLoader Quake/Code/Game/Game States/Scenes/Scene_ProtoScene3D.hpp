#pragma once
#include "Engine/Math/Sphere.hpp"
#include "Engine/Math/ConvexPolyhedron.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Game/Game States/GameState.hpp"
#include "Game/MapParser/MapParser.hpp"
#include "Game/Camera System/Camera Behaviours/CB_FreeLook.hpp"
#include "Game/Camera System/Motion Controllers/CMC_ProportionalController.hpp"

typedef std::vector< Renderable* >	RenderableList;
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

	RenderableList			 m_renderables;

private:
	// Camera
	Camera					*m_camera					= nullptr;
	CameraManager			*m_cameraManager			= nullptr;
	float const				 m_initialFOV				= 45.f;			// Degrees
	float const				 m_cameraNear				= 0.01f;
	float const				 m_cameraFar				= 10000.f;

	// MAP Files
	MapParser				*m_parsedMap				= nullptr;

public:
	void JustFinishedTransition();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render( Camera *gameCamera ) const;

private:
	// Scene Management
	void AddNewLightToScene( Light *light );
	void AddNewRenderableToScene( Renderable *renderable );
};
