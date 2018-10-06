#pragma once
#include <functional>
#include "Game/Abstract Classes/GameState.hpp"

class UIMenu;

class Attract : public GameState
{
public:
	 Attract();
	~Attract();

public:
	void BeginFrame();
	void EndFrame();

	void Update( float deltaSeconds );
	void Render( Camera *gameCamera ) const;

private:
	// Menu Specific
	UIMenu*								 m_attractMenu	= nullptr;
	std::function< void( const char* ) > m_quitStdFunc;
	std::function< void( const char* ) > m_startStdFunc;

	void TransitionToNextState( char const *stateName );

};