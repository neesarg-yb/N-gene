#pragma once
#include <functional>
#include "Game/Abstract Classes/GameState.hpp"

class UIMenu;

class Attract : public GameState
{
public:
	 Attract();
	~Attract();

private:
	// Menu Specific
	UIMenu*								 m_attractMenu	= nullptr;
	std::function< void( const char* ) > quitStdFunc	= std::bind( &theGame::QuitGame,	  this, std::placeholders::_1 );
	std::function< void( const char* ) > startStdFunc	= std::bind( &theGame::GoToMenuState, this, std::placeholders::_1 );

public:
	void BeginFrame();
	void EndFrame();

	void Update( float deltaSeconds );
	void Render( Camera *gameCamera = nullptr ) const;
};