#pragma once
#include <string>
#include "Engine/Core/Clock.hpp"
#include "Game/GameCommon.hpp"

class GameState
{
public:
			 GameState( std::string const &name, Clock const *parentClock = nullptr );
	virtual ~GameState();

public:
	std::string const	 m_name = "NOT ASSIGNED";
	Clock				*m_clock = nullptr;

public:
	virtual void JustFinishedTransition() = 0;			// To this GameState

	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void Update() = 0;
	virtual void Render( Camera *gameCamera ) const = 0;
	
};
