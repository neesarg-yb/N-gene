#pragma once
#include "ProfilerConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfilerReport.hpp"

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

	// Setup Hotkey Info
	std::string hkHeading	= Stringf( " Hotkeys, " );
	std::string tildeStr	= Stringf( "  %-*s: DevConsole ", 6, "[~]" );
	std::string help		= Stringf( "  %-*s: All Commands ", 6, "help" );
	m_hotkeysInfoString		= Stringf( "%s\n%s\n%s", hkHeading.c_str(), tildeStr.c_str(), help.c_str() );
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

	// Profile Report
	ProfileMeasurement* lastFrameMeasure	= Profiler::GetInstance()->GetPreviousFrame();
	ProfileReport lastFrameReport;
	lastFrameReport.GenerateReportFromFrame( lastFrameMeasure );

	std::vector< std::string > reportInStrings;
	lastFrameReport.m_root->GetProfileReportAsStringsVector( reportInStrings, 0 );
	
	// Populate the report string
	m_profileReportString = "";
	for ( uint i = 0; i < reportInStrings.size(); i++ )
		m_profileReportString += ( reportInStrings[i] + "\n " );

	// FPS
	double	secondsPerFrame = Profiler::GetSecondsFromPerformanceCounter( lastFrameReport.m_root->m_totalHPC );
	m_fps					= (int) ( 1.0 / secondsPerFrame );
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
	m_currentRenderer->DrawAABB( m_drawBounds, m_accentColor );
}

void ProfileConsole::Render_FPSBox()
{
	Vector2 padding				 = Vector2( 0.005f, 0.005f );
	Vector2 minBoundPercentage	 = Vector2( 0.f, 1.f - 0.1f );
	Vector2 maxBoundPercentage	 = Vector2( 0.25f, 1.f );
	minBoundPercentage			+= padding;
	maxBoundPercentage			-= padding;

	AABB2	actualBounds		= m_drawBounds.GetBoundsFromPercentage( minBoundPercentage, maxBoundPercentage );

	// Draw Background
	m_currentRenderer->DrawAABB( actualBounds, m_boxBackgroudColor );

	// Draw FPS
	std::string fpsStr = Stringf( "FPS: %03d", m_fps );
	m_currentRenderer->DrawTextInBox2D( fpsStr.c_str(), Vector2( 0.5f, 0.5f ), actualBounds, 0.1f, m_accentColor, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );
}

void ProfileConsole::Render_HotkeysBox()
{
	Vector2 padding				 = Vector2( 0.005f, 0.005f );
	Vector2 minBoundPercentage	 = Vector2( 0.f, 1.f - 0.2f );
	Vector2 maxBoundPercentage	 = Vector2( 0.25f, 1.f - 0.1f );
	minBoundPercentage			+= padding;
	maxBoundPercentage			-= padding;

	AABB2	actualBounds		= m_drawBounds.GetBoundsFromPercentage( minBoundPercentage, maxBoundPercentage );

	// Draw Background
	m_currentRenderer->DrawAABB( actualBounds, m_boxBackgroudColor );

	// Draw Hotkeys
	m_currentRenderer->DrawTextInBox2D( m_hotkeysInfoString.c_str(), Vector2( 0.f, 0.5f ), actualBounds, 0.07f, m_accentColor, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );
}

void ProfileConsole::Render_GraphBox()
{
	Vector2 padding				 = Vector2( 0.005f, 0.005f );
	Vector2 minBoundPercentage	 = Vector2( 0.25f, 1.f - 0.2f );
	Vector2 maxBoundPercentage	 = Vector2( 1.f, 1.f );
	minBoundPercentage			+= padding;
	maxBoundPercentage			-= padding;

	AABB2	actualBounds		= m_drawBounds.GetBoundsFromPercentage( minBoundPercentage, maxBoundPercentage );

	m_currentRenderer->DrawAABB( actualBounds, m_boxBackgroudColor );
}

void ProfileConsole::Render_ProfilingDetailsBox()
{
	Vector2 padding				 = Vector2( 0.005f, 0.005f );
	Vector2 minBoundPercentage	 = Vector2( 0.f, 0.f );
	Vector2 maxBoundPercentage	 = Vector2( 1.f, 1.f - 0.2f );
	minBoundPercentage			+= padding;
	maxBoundPercentage			-= padding;

	AABB2	actualBounds		= m_drawBounds.GetBoundsFromPercentage( minBoundPercentage, maxBoundPercentage );

	// Draw background
	m_currentRenderer->DrawAABB( actualBounds, m_boxBackgroudColor );

	// Draw Report
	if( m_profileReportString != "" )
		m_currentRenderer->DrawTextInBox2D( m_profileReportString.c_str(), Vector2( 0.5f, 1.f ), actualBounds, 0.05f, m_accentColor, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );
}
