#pragma once
#include "Scene_BSPDebug.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"

bool isOnLevelClock_b = true;

Scene_BSPDebug::Scene_BSPDebug( Clock const *parentClock )
	: GameState( "BSP DEBUG", parentClock )
{
	BSPGraph *A = new BSPGraph( "A" );
	BSPGraph *B = new BSPGraph( "B" );
	BSPGraph *C = new BSPGraph( "C" );
	BSPGraph *D = new BSPGraph( "D" );
	BSPGraph *E = new BSPGraph( "E" );
	BSPGraph *F = new BSPGraph( "F" );
	BSPGraph *G = new BSPGraph( "G" );

	BSPGraph *AIR1 = new BSPGraph( "#1" );
	BSPGraph *AIR2 = new BSPGraph( "#2" );
	BSPGraph *AIR3 = new BSPGraph( "#3" );
	BSPGraph *AIR4 = new BSPGraph( "#4" );
	BSPGraph *AIR5 = new BSPGraph( "#5" );
	BSPGraph *AIR6 = new BSPGraph( "#6" );
	
	BSPGraph *SOLID1 = new BSPGraph( "&1" );
	BSPGraph *SOLID2 = new BSPGraph( "&2" );

	A->SetRight( B );
	B->SetRight( AIR1 );
	B->SetLeft( C );
	C->SetRight( AIR2 );
	C->SetLeft( D );
	D->SetRight( AIR3 );
	D->SetLeft( SOLID1 );

	A->SetLeft( E );
	E->SetRight( AIR4 );
	E->SetLeft( F );
	F->SetRight( AIR5 );
	F->SetLeft( G );
	G->SetRight( AIR6 );
	G->SetLeft( SOLID2 );

	m_bspNodes.push_back( A );
	m_bspNodes.push_back( B );
	m_bspNodes.push_back( C );
	m_bspNodes.push_back( D );
	m_bspNodes.push_back( E );
	m_bspNodes.push_back( F );
	m_bspNodes.push_back( G );
	m_bspNodes.push_back( AIR1 );
	m_bspNodes.push_back( AIR2 );
	m_bspNodes.push_back( AIR3 );
	m_bspNodes.push_back( AIR4 );
	m_bspNodes.push_back( AIR5 );
	m_bspNodes.push_back( AIR6 );
	m_bspNodes.push_back( SOLID1 );
	m_bspNodes.push_back( SOLID2 );
}

Scene_BSPDebug::~Scene_BSPDebug()
{
	while( m_bspNodes.size() > 0 )
	{
		std::swap( m_bspNodes.front(), m_bspNodes.back() );
		
		delete m_bspNodes.back();
		m_bspNodes.back() = nullptr;

		m_bspNodes.pop_back();
	}
}

void Scene_BSPDebug::JustFinishedTransition()
{

}

void Scene_BSPDebug::BeginFrame()
{
	DebugRendererBeginFrame( GetMasterClock() );
}

void Scene_BSPDebug::EndFrame()
{

}

void Scene_BSPDebug::Update()
{	
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );
}

void Scene_BSPDebug::Render( Camera *gameCamera ) const
{
	g_theRenderer->BindCamera( gameCamera );
	g_theRenderer->ClearColor( RGBA_BLACK_COLOR );
	g_theRenderer->ClearDepth( 1.f );

	DebugRender2DText( 0.f, Vector2( -850.f, 460.f ), 15.f, RGBA_KHAKI_COLOR, RGBA_KHAKI_COLOR, "Hi!" );
	m_bspNodes[0]->DebugRender( Vector2::ZERO );

	DebugRendererLateRender( nullptr );
}
