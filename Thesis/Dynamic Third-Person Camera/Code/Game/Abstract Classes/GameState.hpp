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
	virtual std::string TransitionToNextState()		const { return "NONE"; }
	virtual std::string TransitionToPreviousState()	const { return "NONE"; }

public:
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void Update( float deltaSeconds ) = 0;
	virtual void Render( Camera *gameCamera = nullptr ) const = 0;
	
};