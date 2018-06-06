#include "theGame.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/DevConsole.hpp"

void EchoTestCommand( Command& cmd )
{
	ConsolePrintf( "%s", cmd.GetNextString().c_str() );
}

theGame::theGame()
{
	m_lastFramesTime = GetCurrentTimeSeconds();

	// Camera Setup
	m_gameCamera = new Camera();
	m_gameCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_gameCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_gameCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );							// To set NDC styled ortho
	g_theRenderer->BindCamera( m_gameCamera );

	// Battle creation
	m_currentBattle = new Battle();

	// Fonts for loading screen
	m_textBmpFont = g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
	
	// Console stuffs
	CommandRegister( "echo", EchoTestCommand );
	ConsolePrintf( RGBA_GREEN_COLOR, "%d Hello World!", 1 );
}

theGame::~theGame()
{
	delete m_textBmpFont;
}

void theGame::BeginFrame()
{
	if( m_currentGameState == BATTLE )
		m_currentBattle->BeginFrame();
}

void theGame::EndFrame()
{
	if( m_currentGameState == BATTLE )
		m_currentBattle->EndFrame();
}

void theGame::Update() 
{
	// Calculating deltaTime
	float deltaSeconds			=	CalculateDeltaTime();
	m_timeSinceTransitionBegan	+=	deltaSeconds;
	m_timeSinceStartOfTheGame	+=	deltaSeconds;
	
	// For UBOTesting..
	g_theRenderer->UpdateTime( deltaSeconds, deltaSeconds );

	// If in transition to another gameState
	if( m_nextGameState != NONE )
	{
		// Update the transition effects' alpha ( FADE OUT )
		float fadeOutFraction	= ClampFloat01( (float)m_timeSinceTransitionBegan / m_halfTransitionTime );		// Goes from 0 to 1
		m_fadeEffectAlpha		= fadeOutFraction;																// Goes from 0 to 1

		// If time to end fade out..
		if( m_timeSinceTransitionBegan >= m_halfTransitionTime )
			ConfirmTransitionToNextState();

		return;
	}

	// If just came in to this gameState..
	if( m_timeSinceTransitionBegan < m_halfTransitionTime )
	{
		// Update the transition effects' alpha ( FADE IN )
		float fadeOutFraction	= ClampFloat01( (float)m_timeSinceTransitionBegan / m_halfTransitionTime );		// Goes from 0 to 1
		m_fadeEffectAlpha		= 1.f - fadeOutFraction;														// Goes from 1 to 0
	}
	else																										// When m_timeSinceTransitionBegan is very big & > m_halfTransitionTime (can happen because of a breakpoint in next gameState)
		m_fadeEffectAlpha		= 0.f;		

	// Continue updating the currentGameState
	switch (m_currentGameState)
	{
	case LOADING:
		Update_Loading( deltaSeconds );
		break;
	case MENU:
		Update_Menu( deltaSeconds );
		break;
	case BATTLE:
		Update_Battle( deltaSeconds );
		break;
	default:
		ERROR_AND_DIE( "Error: No valid gamestate found..! | theGame::Update()" );
		break;
	}
}

void theGame::Render() const
{
	switch (m_currentGameState)
	{
	case LOADING:
		Render_Loading();
		break;
	case MENU:
		Render_Menu();
		break;
	case BATTLE:
		Render_Battle();
		break;
	default:
		ERROR_AND_DIE( "Error: No valid gamestate found..! | theGame::Render()" );
		break;
	}

	// Render the Transition Effects - according to alpha set by Update()
	Rgba overlayBoxColor;
	overlayBoxColor.SetAsFloats( 0.f, 0.f, 0.f, m_fadeEffectAlpha );

	g_theRenderer->BindCamera( m_gameCamera );

	g_theRenderer->UseShader( nullptr );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	g_theRenderer->DrawAABB( m_default_screen_bounds, overlayBoxColor );
}

void theGame::StartTransitionToState( GameStates nextGameState )
{
	m_nextGameState	= nextGameState;

	// Reset appropriate variables for the transitionEffect..
	m_timeSinceTransitionBegan = 0;
}

void theGame::ConfirmTransitionToNextState()
{
	m_currentGameState	= m_nextGameState;
	m_nextGameState		= NONE;

	m_timeSinceTransitionBegan = 0;
}

void theGame::Update_Loading( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if( m_nextGameState == NONE )
		StartTransitionToState( MENU );
}

void theGame::Render_Loading() const
{
	g_theRenderer->BindCamera( m_gameCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );
	
	g_theRenderer->DrawTextInBox2D( "LOADING...", Vector2(0.5f, 0.5f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
}

void theGame::Update_Menu( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if( g_theInput->WasKeyJustPressed( VK_Codes::SPACE ) )
		StartTransitionToState( BATTLE );
	if( g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed )
		StartTransitionToState( BATTLE );
}

void theGame::Render_Menu() const
{
	g_theRenderer->BindCamera( m_gameCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	g_theRenderer->DrawTextInBox2D( "MAIN MENU\n \n Press start\\space button to jump to the game.. \n \n (Press ~ for DevConsole )", Vector2(0.5f, 0.5f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
}

void theGame::Update_Battle( float deltaSeconds )
{
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		StartTransitionToState( MENU );

	m_currentBattle->Update( deltaSeconds );
}

void theGame::Render_Battle() const
{
	m_currentBattle->Render();
}

float theGame::CalculateDeltaTime() {
	double currentTime = GetCurrentTimeSeconds();
	float deltaSeconds = (float)(currentTime - m_lastFramesTime);
	m_lastFramesTime = currentTime;

	return deltaSeconds;
}

double theGame::GetTimeSinceGameStarted() const
{
	return m_timeSinceStartOfTheGame;
}