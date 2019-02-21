#pragma once
#include "MinecraftWorld.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"
#include "Game/World/BlockDefinition.hpp"

MinecraftWorld::MinecraftWorld( Clock const *parentClock, char const *sceneName )
	: GameState( sceneName, parentClock )
{
	BlockDefinition::LoadDefinitions();

	m_world = new World( m_clock );
}

MinecraftWorld::~MinecraftWorld()
{
	delete m_world;
	m_world = nullptr;

	BlockDefinition::DestroyDefinitions();
}

void MinecraftWorld::JustFinishedTransition()
{
	g_theInput->ShowCursor( false );
	g_theInput->SetMouseModeTo( MOUSE_MODE_RELATIVE );
}

void MinecraftWorld::BeginFrame()
{
	// Lock Mouse
	if( g_theInput->WasKeyJustPressed('L') )
	{
		g_theInput->ShowCursor( false );
		g_theInput->SetMouseModeTo( MOUSE_MODE_RELATIVE );
	}

	// Unlock Mouse
	if( g_theInput->WasKeyJustPressed('U') )
	{
		g_theInput->SetMouseModeTo( MOUSE_MODE_ABSOLUTE );
		g_theInput->ShowCursor( true );
	}
}

void MinecraftWorld::EndFrame()
{

}

void MinecraftWorld::Update()
{
	PROFILE_SCOPE_FUNCTION();

	m_world->Update();
	
	// Transition to Level Select if pressed ESC
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
	{
		g_theInput->SetMouseModeTo( MOUSE_MODE_ABSOLUTE );
		g_theInput->ShowCursor( true );

		g_theGame->StartTransitionToState( "LEVEL SELECT" );
	}
}

void MinecraftWorld::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );
	PROFILE_SCOPE_FUNCTION();

	m_world->Render();
}
