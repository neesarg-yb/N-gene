#pragma once
#include <string>
#include "Game/GameCommon.hpp"

class GameState
{
public:
			 GameState( std::string const &name );
	virtual ~GameState();

public:
	std::string const m_name = "NOT ASSIGNED";

public:
	virtual void JustFinishedTransition() = 0;			// To this GameState

	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void Update( float deltaSeconds ) = 0;
	virtual void Render( Camera *gameCamera ) const = 0;
	
};
