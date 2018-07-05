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

	m_currentRenderer->DrawAABB( m_drawBounds, Rgba( 70, 141, 185, 200 ) );
	m_currentRenderer->DrawTextInBox2D( "Profiler..", Vector2( 0.5f, 0.5f ), m_drawBounds, 0.1f, RGBA_BLACK_COLOR, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );
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
