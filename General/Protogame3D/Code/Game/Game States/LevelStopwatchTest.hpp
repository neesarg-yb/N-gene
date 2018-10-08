#pragma once
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Game/Abstract Classes/GameState.hpp"

class LevelStopwatchTest : public GameState
{
public:
	 LevelStopwatchTest();
	~LevelStopwatchTest();

private:
	Clock		*m_levelClock	= nullptr;
	Stopwatch	*m_stopwatch	= nullptr;

public:
	void BeginFrame();
	void EndFrame();

	void Update( float deltaSeconds );
	void Render( Camera *gameCamera ) const;

private:
	void ProcessInput( float deltaSeconds );
	void SwitchTheReferenceClock();
	void PauseTheLevelClock();
	void ResetTheStopwatch();
	void DecrementTheStopwatch();
	void DecrementAllTheStopwatch();
};
