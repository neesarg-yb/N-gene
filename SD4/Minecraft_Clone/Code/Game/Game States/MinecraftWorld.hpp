#pragma once
#include "Engine/Math/Sphere.hpp"
#include "Engine/Core/GameObject.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/ForwardRenderingPath.hpp"
#include "Engine/CameraSystem/CameraManager.hpp"
#include "Game/World/World.hpp"
#include "Game/Game States/GameState.hpp"

class MinecraftWorld : public GameState
{
public:
			 MinecraftWorld( Clock const *parentClock, char const *sceneName );
	virtual ~MinecraftWorld();

private:
	World *m_world = nullptr;

public:
	void JustFinishedTransition();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render( Camera *gameCamera ) const;
};
