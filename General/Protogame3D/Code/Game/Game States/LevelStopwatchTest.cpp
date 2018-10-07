#pragma once
#include "LevelStopwatchTest.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"

LevelStopwatchTest::LevelStopwatchTest()
	: GameState( "LEVEL STOPWATCH" )
{

}

LevelStopwatchTest::~LevelStopwatchTest()
{

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

	DebugRender2DText( 0.f, Vector2::ZERO, 15.f, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, "Testing the Stopwatch" );

	DebugRendererRender();
}
