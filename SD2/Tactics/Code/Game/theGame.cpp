#pragma once
#include "theGame.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/Trajectory.hpp"
#include "Game/SpriteAndAnimation/Sprite.hpp"
#include "Game/SpriteAndAnimation/IsoSprite.hpp"
#include "Game/SpriteAndAnimation/IsoAnimation.hpp"
#include "Game/SpriteAndAnimation/AnimationSet.hpp"

void EchoTestCommand( Command& cmd )
{
	ConsolePrintf( "%s", cmd.GetNextString().c_str() );
}

theGame::theGame()
{
	m_lastFramesTime = GetCurrentTimeSeconds();
	m_textBmpFont	 = g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");

	// Testing grayscale shader..
	m_grayScaleShaderProgram = g_theRenderer->CreateOrGetShaderProgram( "fs_grayscale" );

	// Testing command register..
	CommandRegister( "echo", EchoTestCommand );

	// Testing command parsing with \" \"
	Command testCommand = Command("bla \"bla1 2 3 4\" (5,6,7) \"Hello HelloWorld\" 91011");
	ConsolePrintf( RGBA_GREEN_COLOR, "%d Hello World!", 1 );

	m_gameCamera = new Camera();
	m_gameCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_gameCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_gameCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );							// To set NDC styled ortho
																				//	  set it to identity
	g_theRenderer->SetCurrentCameraTo( m_gameCamera );

	// Loading: Sprites
	Sprite::LoadSpritesFromXML					( "Data\\XML\\Knight\\to_knight_sprite.xml"   );
	IsoSprite::LoadAllIsoSpritesFromXML			( "Data\\XML\\Knight\\knight_f_isosprite.xml" );
	IsoAnimation::LoadAllIsoAnimationsFromXML	( "Data\\XML\\Knight\\knight_f_animation.xml" );
	AnimationSet::LoadAnimationSetFromXML		( "Data\\XML\\Knight\\knight_f_animset.xml"	  );

	Sprite::LoadSpritesFromXML					( "Data\\XML\\Archer\\to_archer_sprite.xml"   );
	IsoSprite::LoadAllIsoSpritesFromXML			( "Data\\XML\\Archer\\archer_f_isosprite.xml" );
	IsoAnimation::LoadAllIsoAnimationsFromXML	( "Data\\XML\\Archer\\archer_f_animation.xml" );
	AnimationSet::LoadAnimationSetFromXML		( "Data\\XML\\Archer\\archer_f_animset.xml"	  );

	Sprite::LoadSpritesFromXML					( "Data\\XML\\Mage\\to_mage_sprite.xml"   );
	IsoSprite::LoadAllIsoSpritesFromXML			( "Data\\XML\\Mage\\mage_f_isosprite.xml" );
	IsoAnimation::LoadAllIsoAnimationsFromXML	( "Data\\XML\\Mage\\mage_f_animation.xml" );
	AnimationSet::LoadAnimationSetFromXML		( "Data\\XML\\Mage\\mage_f_animset.xml"	  );

	// Loading: BlockDefinitions
	XMLDocument blocksDoc;
	const char* xmlDocPathForBlockDef = "Data\\XML\\Blocks.xml";
	blocksDoc.LoadFile( xmlDocPathForBlockDef );
	const XMLElement* blockDefRoot = blocksDoc.RootElement();
	BlockDefinition::LoadBlockDefinations( *blockDefRoot );
	
	// Loading: TileDefinitions
	XMLDocument tileDoc;
	const char* xmlDocPathForTileDef = "Data\\XML\\Tiles.xml";
	tileDoc.LoadFile( xmlDocPathForTileDef );
	const XMLElement* tileDefRoot = tileDoc.RootElement();
	TileDefinition::LoadTileDefinations( *tileDefRoot );

	// Loading: ActorDefinitions
	XMLDocument actorDoc;
	const char* xmlDocPathForActorDef = "Data\\XML\\Actors.xml";
	actorDoc.LoadFile( xmlDocPathForActorDef );
	const XMLElement* actorDefRoot = actorDoc.RootElement();
	ActorDefinition::LoadActorDefinitions( *actorDefRoot );

	m_currentBattle = new Battle();
	g_currentBattle = m_currentBattle;
}

theGame::~theGame()
{
	g_currentBattle = nullptr;
	delete m_currentBattle;

	AnimationSet::DeleteAllAnimationSets();
	IsoAnimation::DeleteAllIsoAnimations();
	IsoSprite::DeleteAllIsoSprites();
	Sprite::DeleteAllSprites();

	ActorDefinition::DeleteAllDefinitions();
	TileDefinition::DeleteAllTheDefinitions();
	BlockDefinition::DeleteAllTheDefinitions();

	delete m_gameCamera;
	delete m_textBmpFont;
}

void theGame::Update() 
{
	// Calculating deltaTime
	float deltaSeconds			=	CalculateDeltaTime();
	m_timeSinceTransitionBegan	+=	deltaSeconds;
	m_timeSinceStartOfTheGame	+=	deltaSeconds;

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
		m_fadeEffectAlpha		= 0.f;																			// make the alpha 0, directly

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

	g_theRenderer->SetCurrentCameraTo( m_gameCamera );
	g_theRenderer->ClearDepth(1.f);
	g_theRenderer->EnableDepth( COMPARE_LESS, false );

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
	g_theRenderer->SetCurrentCameraTo( m_gameCamera );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->ClearDepth( 1.f );
	g_theRenderer->EnableDepth( COMPARE_LESS, true );

	g_theRenderer->DrawTextInBox2D( "LOADING...", Vector2(0.5f, 0.5f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );
}

void theGame::Update_Menu( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	if( g_theInput->WasKeyJustPressed( VK_Codes::SPACE ) )
		StartTransitionToState( BATTLE );
}

void theGame::Render_Menu() const
{
	g_theRenderer->SetCurrentCameraTo( m_gameCamera );

	g_theRenderer->ClearScreen( m_default_screen_color );
	g_theRenderer->ClearDepth( 1.f );
	g_theRenderer->EnableDepth( COMPARE_LESS, true );

	g_theRenderer->DrawTextInBox2D( "MAIN MENU\n \n Press space to start the game.. \n \n (Press ~ for DevConsole )", Vector2(0.5f, 0.5f), m_default_screen_bounds, 0.08f, RGBA_RED_COLOR, m_textBmpFont, TEXT_DRAW_SHRINK_TO_FIT );

	g_theRenderer->SetPassFloatForShaderTo( (float)m_timeSinceStartOfTheGame );
	g_theRenderer->ApplyEffect( m_grayScaleShaderProgram );
	g_theRenderer->EndEffect();
	g_theRenderer->ResetPassFloatForShaderToZero();
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