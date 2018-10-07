#pragma once
#include "Game/Abstract Classes/GameState.hpp"

class LevelStopwatchTest : public GameState
{
public:
	 LevelStopwatchTest();
	~LevelStopwatchTest();

public:
	void BeginFrame();
	void EndFrame();

	void Update( float deltaSeconds );
	void Render( Camera *gameCamera ) const;
};