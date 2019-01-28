#pragma once
#include <functional>
#include "Engine/Core/UIMenu.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game States/GameState.hpp"

class LevelSelect : public GameState
{
public:
	 LevelSelect( Clock const *parentClock );
	~LevelSelect();

public:
	void JustFinishedTransition();

	void BeginFrame();
	void EndFrame();

	void Update();
	void Render( Camera *gameCamera ) const;

private:
	// Level Selection UI
	UIMenu*									m_levelSelectionMenu	= nullptr;
	std::function< void( const char* ) >	m_levelSelectedStdFunc	= std::bind( &LevelSelect::LevelSelected, this, std::placeholders::_1 );

	void LevelSelected( char const * levelName );
};