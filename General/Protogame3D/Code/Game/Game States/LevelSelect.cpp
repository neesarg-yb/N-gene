#pragma once
#include "LevelSelect.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Core/MenuAction.hpp"
#include "Game/theGame.hpp"

LevelSelect::LevelSelect( Clock const *parentClock )
	: GameState( "LEVEL SELECT", parentClock )
{
	// Setup the LevelSelection UI
	m_levelSelectionMenu = new UIMenu( *g_theInput, *g_theRenderer, AABB2( 0.35f, 0.45f, 0.65f, 0.55f ) );
	m_levelSelectionMenu->AddNewMenuAction( MenuAction("--> Protoscene 3D               ", &m_levelSelectedStdFunc) );
	m_levelSelectionMenu->AddNewMenuAction( MenuAction("--> Quaternion Basis             ", &m_levelSelectedStdFunc) );
	m_levelSelectionMenu->AddNewMenuAction( MenuAction("--> Clock & Stopwatch            ", &m_levelSelectedStdFunc) );
	m_levelSelectionMenu->m_selectionIndex = 0;
}

LevelSelect::~LevelSelect()
{

}

void LevelSelect::JustFinishedTransition()
{

}

void LevelSelect::BeginFrame()
{

}

void LevelSelect::EndFrame()
{

}

void LevelSelect::Update()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();

	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "ATTRACT" );

	m_levelSelectionMenu->Update( deltaSeconds );
}

void LevelSelect::Render( Camera *gameCamera ) const
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	g_theRenderer->BindCamera( gameCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->ClearScreen( g_theGame->m_default_screen_color );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	g_theRenderer->DrawTextInBox2D( "Scenes", Vector2(0.5f, 0.6f), g_theGame->m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, g_theGame->m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
	g_theRenderer->DrawTextInBox2D( "(Press ~ for DevConsole )", Vector2(0.5f, 0.02f), g_theGame->m_default_screen_bounds, 0.035f, RGBA_RED_COLOR, g_theGame->m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );

	m_levelSelectionMenu->Render();
}

void LevelSelect::LevelSelected( char const * levelName )
{
	if( std::string(levelName) == "--> Clock & Stopwatch            " )
		g_theGame->StartTransitionToState( "CLOCK & STOPWATCH" );
	else if( std::string(levelName) == "--> Quaternion Basis             " )
		g_theGame->StartTransitionToState( "QUATERNIONS TEST" );
	else if( std::string(levelName) == "--> Protoscene 3D               " )
		g_theGame->StartTransitionToState( "PROTOSCENE 3D" );
}
