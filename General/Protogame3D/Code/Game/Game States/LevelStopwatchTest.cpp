#pragma once
#include "LevelStopwatchTest.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"

LevelStopwatchTest::LevelStopwatchTest()
	: GameState( "LEVEL STOPWATCH" )
{
	m_levelClock = new Clock( GetMasterClock() );
}

LevelStopwatchTest::~LevelStopwatchTest()
{
	delete m_levelClock;
	m_levelClock = nullptr;
}

void LevelStopwatchTest::BeginFrame()
{

}

void LevelStopwatchTest::EndFrame()
{

}

void LevelStopwatchTest::Update( float deltaSeconds )
{
	DebugRendererUpdate( deltaSeconds );
	
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );
}

void LevelStopwatchTest::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );

	DebugRender2DText( 0.f, Vector2( -850.f, 460.f ), 15.f, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, "Testing the Stopwatch" );
	
//	std::string currentLevelTimeStr = m_levelClock->GetCurrentTimeStamp();
//	DebugRender2DText( 0.f, Vector2( -850.f, 440.f ), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, currentLevelTimeStr.c_str() );

	DebugRendererRender();
}
