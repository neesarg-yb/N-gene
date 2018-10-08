#pragma once
#include "Engine/Core/Clock.hpp"
#include "Game/Abstract Classes/GameState.hpp"

class LevelStopwatchTest : public GameState
{
public:
	 LevelStopwatchTest();
	~LevelStopwatchTest();

private:
	Clock *m_levelClock = nullptr;

public:
	void BeginFrame();
	void EndFrame();

	void Update( float deltaSeconds );
	void Render( Camera *gameCamera ) const;
};
