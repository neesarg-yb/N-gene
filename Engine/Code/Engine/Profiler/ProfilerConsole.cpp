#pragma once
#include "ProfilerConsole.hpp"
#include "Engine/Math/BarGraph.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Profiler/ProfilerReport.hpp"
#include "Engine/Input/Command.hpp"

ProfileConsole* ProfileConsole::s_profileConsoleInstance = nullptr;

eProfileReportType currentReportFormat	= PROFILE_REPORT_TREE;
eProfileReportSort currentFlatSorting	= PROFILE_REPORT_SORT_SELF_TIME;

void ChangeProfileReportDisplayFormat()
{
	int i = (int) currentReportFormat;
	i = i + 1;
	i = i % (int)NUM_PROFILE_REPORT_TYPES;

	currentReportFormat = (eProfileReportType) i;
}

void ChangeSortingOnFlatReport()
{
	int i = (int) currentFlatSorting;
	i = i + 1;
	i = i % (int)NUM_PROFILE_REPORT_SORTS;

	currentFlatSorting = (eProfileReportSort) i;
}

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
	std::string reportFrmt	= Stringf( "  %-*s: Report Format ", 6, "[F]" );
	std::string flatSort	= Stringf( "  %-*s: Flat Sort Format ", 6, "[S]" );
	m_hotkeysInfoString		= Stringf( "%s\n%s\n%s\n%s", hkHeading.c_str(), tildeStr.c_str(), reportFrmt.c_str(), flatSort.c_str() );

	// Graph
	m_frameGraph = new BarGraph( 128 );
}

ProfileConsole::~ProfileConsole()
{
	delete m_frameGraph;
}

ProfileConsole* ProfileConsole::GetInstance()
{
	if( s_profileConsoleInstance == nullptr )
		s_profileConsoleInstance = new ProfileConsole( Renderer::GetInstance() );

	return s_profileConsoleInstance;
}

void ProfileConsole::Update( InputSystem& currentInputSystem )
{
	uint64_t	thisFramesHPC	= Profiler::GetPerformanceCounter();
	uint64_t	deltaHPC		= thisFramesHPC - m_lastFramesHPC;
	m_frameTime					= Profiler::GetSecondsFromPerformanceCounter( deltaHPC );
	m_lastFramesHPC				= thisFramesHPC;

	if( IsOpen() == false )
		return;

	// change report format
	if( currentInputSystem.WasKeyJustPressed( 'F' ) )
		ChangeProfileReportDisplayFormat();

	// change sorting
	if( currentInputSystem.WasKeyJustPressed( 'S' ) )
		ChangeSortingOnFlatReport();

	// Profile Report
	ProfileMeasurement* lastFrameMeasure	= Profiler::GetInstance()->GetPreviousFrame();
	ProfileReport lastFrameReport;
	lastFrameReport.GenerateReportFromFrame( lastFrameMeasure, currentReportFormat );

	// Add frame to graph
	if( m_frameCountUptoSkip < m_skipFramesForGraph )	// Skipping frames: Because several initial frames can produce very high totalFrameTimes
		m_frameCountUptoSkip++;
	else
		m_frameGraph->AppendDataPoint( Profiler::GetMillliSecondsFromPerformanceCounter( lastFrameReport.m_root->m_totalHPC ) );

	// Do sorting on flat report
	if( currentReportFormat == PROFILE_REPORT_FLAT )
	{
		if( currentFlatSorting == PROFILE_REPORT_SORT_TOTAL_TIME )
			lastFrameReport.SortByTotalTime();
		else
			lastFrameReport.SortBySelfTime();
	}

	std::vector< std::string > reportInStrings;
	lastFrameReport.m_root->GetProfileReportAsStringsVector( reportInStrings, 0 );
	
	// Populate the report string
	m_profileReportString = "";
	for ( uint i = 0; i < reportInStrings.size(); i++ )
		m_profileReportString += ( reportInStrings[i] + "\n " );

	// FPS
	m_fps = (int) ( 1.0 / m_frameTime );
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
	std::string fpsNum			= Stringf( "%03d", m_fps );
	std::string frameTimeNum	= Stringf( "%07.3f", m_frameTime * 1000.0  );
	std::string fpsStr			= Stringf( " %*s: %*s ",	-15, "FPS",				-7, fpsNum.c_str() );
	std::string frameTimeStr	= Stringf( " %*s: %*s ",	-15, "Frame Time(MS)",	-7, frameTimeNum.c_str() );
	std::string finalStr		= Stringf( "%s\n%s", fpsStr.c_str(), frameTimeStr.c_str() );
	m_currentRenderer->DrawTextInBox2D( finalStr.c_str(), Vector2( 0.5f, 0.5f ), actualBounds, 0.1f, m_accentColor, m_fonts, TEXT_DRAW_SHRINK_TO_FIT );
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

	AABB2	actualBounds = m_drawBounds.GetBoundsFromPercentage( minBoundPercentage, maxBoundPercentage );
	m_currentRenderer->DrawAABB( actualBounds, m_boxBackgroudColor );
	
	AABB2	graphBounds	= actualBounds.GetBoundsFromPercentage( Vector2( 0.01f, 0.05f ), Vector2( 0.99f, 0.95f ) );
	Mesh *	graphMesh	= m_frameGraph->CreateVisualGraphMesh( graphBounds, m_accentColor );
	m_currentRenderer->SetCurrentDiffuseTexture( nullptr );
	m_currentRenderer->DrawMesh( *graphMesh );
	delete graphMesh;
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
