#pragma once
#include "Scene_StopwatchTest.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"

bool isOnLevelClock = true;

Scene_StopwatchTest::Scene_StopwatchTest( Clock const *parentClock )
	: GameState( "CLOCK & STOPWATCH", parentClock )
{
	m_levelClock	= new Clock( parentClock );
	m_stopwatch		= new Stopwatch( m_levelClock );
	m_stopwatch->SetTimer( 10 );
}

Scene_StopwatchTest::~Scene_StopwatchTest()
{
	delete m_stopwatch;
	m_stopwatch = nullptr;

	delete m_levelClock;
	m_levelClock = nullptr;
}

void Scene_StopwatchTest::JustFinishedTransition()
{

}

void Scene_StopwatchTest::BeginFrame()
{
	DebugRendererBeginFrame( GetMasterClock() );
}

void Scene_StopwatchTest::EndFrame()
{

}

void Scene_StopwatchTest::Update()
{
	ProcessInput( (float)GetMasterClock()->GetFrameDeltaSeconds() );
	
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );
}

void Scene_StopwatchTest::Render( Camera *gameCamera ) const
{
	g_theRenderer->BindCamera( gameCamera );
	g_theRenderer->ClearColor( RGBA_BLACK_COLOR );
	g_theRenderer->ClearDepth( 1.f );

	DebugRender2DText( 0.f, Vector2( -850.f, 460.f ), 15.f, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, "Testing the Stopwatch" );
	DebugRender2DText( 0.f, Vector2( -850.f, 440.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "[S] : Switch the Reference Clock" );
	DebugRender2DText( 0.f, Vector2( -850.f, 420.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "[P] : Pause the Level Clock" );
	DebugRender2DText( 0.f, Vector2( -850.f, 400.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "[R] : Reset the Stopwatch" );
	DebugRender2DText( 0.f, Vector2( -850.f, 380.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "[D] : Decrement Stopwatch, Once!" );
	DebugRender2DText( 0.f, Vector2( -850.f, 360.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "[SHIFT] + [D]  : Decrement Stopwatch, All!" );
	DebugRender2DText( 0.f, Vector2( -850.f, 340.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, "[UP] or [DOWN] : Change the TimeScale of Level Clock!" );

	double	intervalSeconds			= m_stopwatch->GetIntervalSeconds();
	double	elapsedTime				= m_stopwatch->GetElapsedTime();
	float	normalizedElapsedTime	= m_stopwatch->GetNormalizedElapsedTime();
	bool	hasElapsed				= m_stopwatch->HasElapsed();

	DebugRender2DText( 0.f, Vector2( -850.f, -300.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf("Level Clock             : %s", (m_levelClock->IsPaused() ? "Paused" : "Running..")) );
	DebugRender2DText( 0.f, Vector2( -850.f, -320.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf("Level Clock TimeScale   : %f",  m_levelClock->GetTimeScale()) );
	DebugRender2DText( 0.f, Vector2( -850.f, -340.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf("Reference Clock         : %s", (isOnLevelClock ? "Level Clock" : "Master Clock")) );
	DebugRender2DText( 0.f, Vector2( -850.f, -360.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf("Interval(s)             : %s", std::to_string(intervalSeconds).c_str()) );
	DebugRender2DText( 0.f, Vector2( -850.f, -380.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf("Elapsed Time(s)         : %s", std::to_string(elapsedTime).c_str()) );
	DebugRender2DText( 0.f, Vector2( -850.f, -400.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf("Normalized Elapsed Time : %s", std::to_string(normalizedElapsedTime).c_str()) );
	DebugRender2DText( 0.f, Vector2( -850.f, -420.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, Stringf("Has Interval Elapsed?   : %s", (hasElapsed ? "YES" : "NO")) );

	DebugRendererLateRender( nullptr );
}

void Scene_StopwatchTest::ProcessInput( float deltaSeconds )
{
	if( g_theInput->WasKeyJustPressed( 'S' ) )
		SwitchTheReferenceClock();
	if( g_theInput->WasKeyJustPressed( 'P' ) )
		PauseTheLevelClock();
	if( g_theInput->WasKeyJustPressed( 'R' ) )
		ResetTheStopwatch();
	if( g_theInput->WasKeyJustPressed( 'D' ) && !g_theInput->IsKeyPressed( VK_Codes::SHIFT ) )		// Just D
		DecrementTheStopwatch();
	if( g_theInput->WasKeyJustPressed( 'D' ) && g_theInput->IsKeyPressed( VK_Codes::SHIFT ) )		// Shift + D
		DecrementAllTheStopwatch();
	if( g_theInput->IsKeyPressed( VK_Codes::UP ) )
		IncrementLevelClockTimeScale( deltaSeconds );
	if( g_theInput->IsKeyPressed( VK_Codes::DOWN ) )
		DecrementLevelClockTimeScale( deltaSeconds );
}

float timescaleChangePerSeconds = 0.15f;
void Scene_StopwatchTest::IncrementLevelClockTimeScale( float deltaSeconds )
{
	double currentTS = m_levelClock->GetTimeScale();
	currentTS		+= timescaleChangePerSeconds * deltaSeconds;

	m_levelClock->SetTimeSclae( currentTS );
}

void Scene_StopwatchTest::DecrementLevelClockTimeScale( float deltaSeconds )
{
	double currentTS = m_levelClock->GetTimeScale();
	currentTS		-= timescaleChangePerSeconds * deltaSeconds;

	m_levelClock->SetTimeSclae( currentTS );
}

void Scene_StopwatchTest::SwitchTheReferenceClock()
{
	if( isOnLevelClock )
		m_stopwatch->SetClock( nullptr );
	else
		m_stopwatch->SetClock( m_levelClock );

	isOnLevelClock = !isOnLevelClock;
}

void Scene_StopwatchTest::PauseTheLevelClock()
{
	if( m_levelClock->IsPaused() )
		m_levelClock->Resume();
	else
		m_levelClock->Pause();
}

void Scene_StopwatchTest::ResetTheStopwatch()
{
	m_stopwatch->Reset();
}

void Scene_StopwatchTest::DecrementTheStopwatch()
{
	bool decrementSuccess = m_stopwatch->Decrement();

	DebugRender2DText( 3.f, Vector2( -850.f, -460.f ), 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("%s", (decrementSuccess ? "Decremented one time!" : "No Decrements.")) );
}

void Scene_StopwatchTest::DecrementAllTheStopwatch()
{
	uint decrementCount = m_stopwatch->DecrementAll();

	DebugRender2DText( 3.f, Vector2( -850.f, -460.f ), 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, Stringf("Decrement Count: %d", decrementCount) );
}
