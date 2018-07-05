#pragma once
#include "ProfilerConsole.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"

ProfileConsole* ProfileConsole::s_profileConsoleInstance = nullptr;

ProfileConsole::ProfileConsole( Renderer* currentRenderer )
	: m_currentRenderer( currentRenderer )
{
	// Fonts
	m_fonts = currentRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");

	// Camera
	m_profileConsoleCamera = new Camera();

	// Setting up the Camera
	m_profileConsoleCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_profileConsoleCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_profileConsoleCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );			// Make an NDC
}

ProfileConsole::~ProfileConsole()
{

}

ProfileConsole* ProfileConsole::GetInstance()
{
	if( s_profileConsoleInstance == nullptr )
		s_profileConsoleInstance = new ProfileConsole( Renderer::GetInstance() );

	return s_profileConsoleInstance;
}

void ProfileConsole::Update( InputSystem& currentInputSystem )
{
	if( IsOpen() == false )
		return;

	UNUSED( currentInputSystem );
}

void ProfileConsole::Render()
{
	if( IsOpen() == false )
		return;

	m_currentRenderer->BindCamera( m_profileConsoleCamera );

	m_currentRenderer->UseShader( nullptr );
	m_currentRenderer->EnableDepth( COMPARE_ALWAYS, false );

	Render_Backgroud();
	Render_FPSBox();
	Render_HotkeysBox();
	Render_GraphBox();
	Render_ProfilingDetailsBox();
}

void ProfileConsole::Open()
{
	m_isOpen = true;
}

void ProfileConsole::Close()
{
	m_isOpen = false;
}

bool ProfileConsole::IsOpen()
{
	return m_isOpen;
}

void ProfileConsole::Render_Backgroud()
{
	// Blue background
	m_currentRenderer->DrawAABB( m_drawBounds, Rgba( 70, 141, 185, 200 ) );
}

void ProfileConsole::Render_FPSBox()
{
	Vector2 padding				 = Vector2( 0.005f, 0.005f );
	Vector2 minBoundPercentage	 = Vector2( 0.f, 1.f - 0.1f );
	Vector2 maxBoundPercentage	 = Vector2( 0.25f, 1.f );
	minBoundPercentage			+= padding;
	maxBoundPercentage			-= padding;

	AABB2	actualBounds		= m_drawBounds.GetBoundsFromPercentage( minBoundPercentage, maxBoundPercentage );

	m_currentRenderer->DrawAABB( actualBounds, Rgba( 0, 0, 0, 180 ) );
}

void ProfileConsole::Render_HotkeysBox()
{
	Vector2 padding				 = Vector2( 0.005f, 0.005f );
	Vector2 minBoundPercentage	 = Vector2( 0.f, 1.f - 0.2f );
	Vector2 maxBoundPercentage	 = Vector2( 0.25f, 1.f - 0.1f );
	minBoundPercentage			+= padding;
	maxBoundPercentage			-= padding;

	AABB2	actualBounds		= m_drawBounds.GetBoundsFromPercentage( minBoundPercentage, maxBoundPercentage );

	m_currentRenderer->DrawAABB( actualBounds, Rgba( 0, 0, 0, 180 ) );
}

void ProfileConsole::Render_GraphBox()
{
	Vector2 padding				 = Vector2( 0.005f, 0.005f );
	Vector2 minBoundPercentage	 = Vector2( 0.25f, 1.f - 0.2f );
	Vector2 maxBoundPercentage	 = Vector2( 1.f, 1.f );
	minBoundPercentage			+= padding;
	maxBoundPercentage			-= padding;

	AABB2	actualBounds		= m_drawBounds.GetBoundsFromPercentage( minBoundPercentage, maxBoundPercentage );

	m_currentRenderer->DrawAABB( actualBounds, Rgba( 0, 0, 0, 180 ) );
}

void ProfileConsole::Render_ProfilingDetailsBox()
{
	Vector2 padding				 = Vector2( 0.005f, 0.005f );
	Vector2 minBoundPercentage	 = Vector2( 0.f, 0.f );
	Vector2 maxBoundPercentage	 = Vector2( 1.f, 1.f - 0.2f );
	minBoundPercentage			+= padding;
	maxBoundPercentage			-= padding;

	AABB2	actualBounds		= m_drawBounds.GetBoundsFromPercentage( minBoundPercentage, maxBoundPercentage );

	m_currentRenderer->DrawAABB( actualBounds, Rgba( 0, 0, 0, 180 ) );
}
