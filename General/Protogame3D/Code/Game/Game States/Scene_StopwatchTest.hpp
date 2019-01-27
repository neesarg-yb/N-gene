#pragma once
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Game/Abstract Classes/GameState.hpp"

class Scene_StopwatchTest : public GameState
{
public:
	 Scene_StopwatchTest( Clock const *parentClock );
	~Scene_StopwatchTest();

private:
	Clock		*m_levelClock	= nullptr;
	Stopwatch	*m_stopwatch	= nullptr;

public:
	void JustFinishedTransition();

	void BeginFrame();
	void EndFrame();

	void Update();
	void Render( Camera *gameCamera ) const;

private:
	void ProcessInput( float deltaSeconds );
	void IncrementLevelClockTimeScale( float deltaSeconds );
	void DecrementLevelClockTimeScale( float deltaSeconds );
	void SwitchTheReferenceClock();
	void PauseTheLevelClock();
	void ResetTheStopwatch();
	void DecrementTheStopwatch();
	void DecrementAllTheStopwatch();
};
