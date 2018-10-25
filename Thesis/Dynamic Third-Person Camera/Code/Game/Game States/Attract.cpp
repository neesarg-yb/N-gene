#pragma once
#include "Attract.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Core/UIMenu.hpp"
#include "Game/theGame.hpp"

Attract::Attract( Clock const *parentClock )
	: GameState( "ATTRACT", parentClock )
{
	// Setting up the Attract Menu
	m_attractMenu	= new UIMenu( *g_theInput, *g_theRenderer, AABB2( 0.45f, 0.45f, 0.55f, 0.55f ) );
	m_quitStdFunc	= std::bind( &theGame::QuitGame,				g_theGame,	std::placeholders::_1 );
	m_startStdFunc	= std::bind( &Attract::TransitionToNextState,	this,		std::placeholders::_1 );

	m_attractMenu->AddNewMenuAction( MenuAction( "Quit", &m_quitStdFunc ) );
	m_attractMenu->AddNewMenuAction( MenuAction( "Start", &m_startStdFunc ) );
	m_attractMenu->m_selectionIndex = 1;
}

Attract::~Attract()
{
	
}

void Attract::JustFinishedTransition()
{

}

void Attract::BeginFrame()
{

}

void Attract::EndFrame()
{

}

void Attract::Update()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();

	// Menu handles the state transition
	m_attractMenu->Update( deltaSeconds );
}

void Attract::Render( Camera *gameCamera ) const
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Title
	g_theRenderer->BindCamera( gameCamera );
	g_theRenderer->UseShader( nullptr );
	
	g_theRenderer->ClearScreen( RGBA_BLACK_COLOR );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );
	
	g_theRenderer->DrawTextInBox2D( "Dynamic Third-Person Camera", Vector2(0.5f, 0.6f), g_theGame->m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, g_theGame->m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
	g_theRenderer->DrawTextInBox2D( "( Use Keyboard )", Vector2(0.5f, 0.02f), g_theGame->m_default_screen_bounds, 0.035f, RGBA_RED_COLOR, g_theGame->m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );

	// Menu
	m_attractMenu->Render();
}

void Attract::TransitionToNextState( char const *stateName )
{
	UNUSED( stateName );

	g_theGame->StartTransitionToState( "LEVEL SELECT" );
}
