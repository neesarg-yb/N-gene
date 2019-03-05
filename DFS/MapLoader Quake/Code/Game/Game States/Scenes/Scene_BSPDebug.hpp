#pragma once
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Game/BSP/BSPGraph.hpp"
#include "Game/Game States/GameState.hpp"

class Scene_BSPDebug : public GameState
{
public:
	 Scene_BSPDebug( Clock const *parentClock );
	~Scene_BSPDebug();

private:
	std::vector< BSPGraph* > m_bspNodes;

public:
	void JustFinishedTransition();

	void BeginFrame();
	void EndFrame();

	void Update();
	void Render( Camera *gameCamera ) const;
};
