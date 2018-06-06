#include "theGame.hpp"
#include "Game/AIBehavior.hpp"


theGame::theGame()
{
	m_lastFramesTime = GetCurrentTimeSeconds();

	Texture* tileSSTexture = g_theRenderer->CreateOrGetTexture("Data\\Images\\Terrain_32x32.png");
	g_tileSpriteSheet = new SpriteSheet( *tileSSTexture, 32, 32 );

	// Tile XML Test
	XMLDocument tileDoc;
	const char* xmlDocPathBB = "Data\\Definitions\\Tiles.xml"; 
	tileDoc.LoadFile( xmlDocPathBB );

	const XMLElement* root1 = tileDoc.RootElement();
	// Populate the TileDefinations
	TileDefinition::LoadTileDefinitions( *root1 );

	// Tile XML Test
	XMLDocument tileDefXML;
	const char* xmlDocPathTileDef = "Data\\Definitions\\Maps.xml"; 
	tileDefXML.LoadFile( xmlDocPathTileDef );

	const XMLElement* root2 = tileDefXML.RootElement();
	// Populate the TileDefinations
	MapDefinition::LoadMapDefinitions( *root2 );

	std::vector< std::string > strings;
	SetFromText( strings, ",", "1,2,13,144,1555,16,7,8,9" );

	// Testing vector<int> Parse function
	std::vector<int> numbers = ParseXmlAttribute( *tileDoc.RootElement(), "testIntVec", numbers );

	// Testing SpriteAnimSetDef..
	XMLDocument actorDoc;
	const char* xmlDocPathActorDoc = "Data\\Definitions\\Actors.xml";
	actorDoc.LoadFile( xmlDocPathActorDoc );

	const XMLElement* animSetElement = actorDoc.RootElement();
	ActorDefinition::LoadDefinitions( *animSetElement );

	// Testing ProjectileDefinition..
	XMLDocument projectileDoc;
	const char* xmlDocPathProjDoc = "Data\\Definitions\\Projectiles.xml";
	projectileDoc.LoadFile( xmlDocPathProjDoc );

	const XMLElement* projRootElement = projectileDoc.RootElement();
	ProjectileDefinition::LoadDefinitions( *projRootElement );
	
	// Testing PortalDefinition..
	XMLDocument portalDoc;
	const char* xmlDocPathPortalDoc = "Data\\Definitions\\Portals.xml";
	portalDoc.LoadFile( xmlDocPathPortalDoc );

	const XMLElement* portalRootElement = portalDoc.RootElement();
	PortalDefinition::LoadDefinitions( *portalRootElement );
	
	// Testing AdventureDefinition..
	XMLDocument adventureDoc;
	const char* xmlDocPathAdventureDoc = "Data\\Definitions\\Adventures.xml";
	adventureDoc.LoadFile( xmlDocPathAdventureDoc );

	const XMLElement* adventureRootElement = adventureDoc.RootElement();
	AdventureDefinition::LoadDefinitions( *adventureRootElement );

	// Testing the Entity..
	m_potentialPlayer = new Player( Vector2( 1.5f, 1.5f ), 90.f, std::string("Player") );

	// Create current Map
	m_currentAdventure = new Adventure( std::string("testAdventure") , std::string("Balrog"), *m_potentialPlayer );

	// Testing the Camera
	m_theCamera = new Camera2D( *m_potentialPlayer, *m_currentAdventure->m_currentMap, 10.f, 16.f/9.f );

	// Testing bitmapFont
	newBMF = g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
}

theGame::~theGame()
{
	
}

void theGame::BeginFrame()
{
	if(m_currentState == PLAYING )
		m_currentAdventure->BeginFrame();
}

void theGame::EndFrame()
{
	if(m_currentState == PLAYING )
		m_currentAdventure->EndFrame();
}

void theGame::Update() {

	// Calculating deltaTime
	float deltaSeconds = CalculateDeltaTime();

	// If in transition
	if( m_transitionToState != NONE )
	{
		m_secondsInStateTransition += deltaSeconds;
		return;
	}
	else
	{
		m_secondsInCurrentState += deltaSeconds;
	}

	// Switch to the currentState
	switch (m_currentState)
	{
	case ATTRACT:	Update_Attract(deltaSeconds);	break;
	case PLAYING:	Update_Playing(deltaSeconds);	break;
	case PAUSED:	Update_Paused(deltaSeconds);	break;
	case DEFEAT:	Update_Defeat(deltaSeconds);	break;
	case VICTORY:	Update_Victory(deltaSeconds);	break;
	case NONE:
		GUARANTEE_OR_DIE( false, std::string("Update(): Current GameState can't be NONE") );
		break;
	case NUM_GAME_STATES:
		GUARANTEE_OR_DIE( false, std::string("Update(): Current GameState can't be NUM_GAME_STATES") );
		break;
	}
}

void theGame::Render() {
	// Switch to the currentState
	switch (m_currentState)
	{
	case ATTRACT:	Render_Attract();	break;
	case PLAYING:	Render_Playing();	break;
	case PAUSED:	Render_Paused();	break;
	case DEFEAT:	Render_Defeat();	break;
	case VICTORY:	Render_Victory();	break;
	case NONE:
		GUARANTEE_OR_DIE( false, std::string("Render(): Current GameState can't be NONE") );
		break;
	case NUM_GAME_STATES:
		GUARANTEE_OR_DIE( false, std::string("Render(): Current GameState can't be NUM_GAME_STATES") );
		break;
	}


	// If in transition
	if( m_transitionToState != NONE )
	{
		// Do fade out for..
		if( m_secondsInStateTransition < m_default_overlay_time )
		{
			float overlayfraction = m_secondsInStateTransition / m_default_overlay_time;
			m_theCamera->OverlaySolidAABBOnScreenWithAlpha( RGBA_BLACK_COLOR, 255.f * overlayfraction );
		}
		// After fade out, change the scene
		else
		{
			m_currentState = m_transitionToState;
			m_transitionToState = NONE;
			m_secondsInCurrentState = 0.f;
			m_isFinishedTransitioning = true;
		}
	}
	else
	{
		// Do fade in..
		if( m_secondsInCurrentState < m_default_overlay_time )
		{
			float overlayfraction = 1.f - (m_secondsInCurrentState / m_default_overlay_time);
			m_theCamera->OverlaySolidAABBOnScreenWithAlpha( RGBA_BLACK_COLOR, 255.f * overlayfraction );
		}
	}
}

void theGame::StartTransitionToState( GameState nextGameState )
{
	m_isFinishedTransitioning	= false;
	m_secondsInStateTransition	= 0.f;
	m_transitionToState			= nextGameState;
}

void theGame::Update_Attract( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	m_theCamera->Update(m_default_ortho_leftBottom, m_default_ortho_topRight);

	if (g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed)
		StartTransitionToState( PLAYING );
}

void theGame::Update_Playing( float deltaSeconds )
{
	m_currentAdventure->Update(deltaSeconds);
	m_theCamera->Update();

	if (g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed)
		StartTransitionToState( PAUSED );
	
	if (g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_LB ].keyJustPressed)
		StartTransitionToState( DEFEAT );

	if (g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_RB ].keyJustPressed)
		StartTransitionToState( VICTORY );
}

void theGame::Update_Paused ( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	m_theCamera->Update(m_default_ortho_leftBottom, m_default_ortho_topRight);

	if (g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed)
		StartTransitionToState( PLAYING );
}

void theGame::Update_Defeat ( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	// Reset Player's Health, on defeat..
	if( m_potentialPlayer->m_readyToDie == true )
		m_potentialPlayer->ResetHealth();

	m_theCamera->Update(m_default_ortho_leftBottom, m_default_ortho_topRight);

	if (g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed)
		StartTransitionToState( ATTRACT );
}

void theGame::Update_Victory( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	m_theCamera->Update(m_default_ortho_leftBottom, m_default_ortho_topRight);

	if (g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed)
		StartTransitionToState( ATTRACT );
}

void theGame::Render_Attract()
{
	m_theCamera->Render();

	AABB2 boxForText = AABB2( 4.f, 2.25f, 12.f, 6.75f );
	g_theRenderer->DrawTextInBox2D( std::string("Adventure!"), Vector2(0.5f, 0.5f), boxForText, 
		0.7f, RGBA_RED_COLOR, newBMF, TEXT_DRAW_WORD_WRAP );
	g_theRenderer->DrawTextInBox2D( std::string("PRESS START TO PLAY"), Vector2(0.5f, 0.0f), boxForText, 
		0.2f, RGBA_RED_COLOR, newBMF, TEXT_DRAW_SHRINK_TO_FIT );
}

void theGame::Render_Playing()
{
	m_theCamera->Render();
	m_currentAdventure->Render();
}

void theGame::Render_Paused ()
{
	m_theCamera->Render();

	AABB2 boxForText = AABB2( 4.f, 2.25f, 12.f, 6.75f );
	g_theRenderer->DrawTextInBox2D( std::string("Game Paused!"), Vector2(0.5f, 0.5f), boxForText, 
		0.7f, RGBA_RED_COLOR, newBMF, TEXT_DRAW_SHRINK_TO_FIT );
	g_theRenderer->DrawTextInBox2D( std::string("PRESS START TO CONTINUE"), Vector2(0.5f, 0.0f), boxForText, 
		0.2f, RGBA_RED_COLOR, newBMF, TEXT_DRAW_SHRINK_TO_FIT );
}

void theGame::Render_Defeat ()
{
	m_theCamera->Render();

	AABB2 boxForText = AABB2( 4.f, 2.25f, 12.f, 6.75f );
	g_theRenderer->DrawTextInBox2D( std::string("Defeat..!"), Vector2(0.5f, 0.5f), boxForText, 
		0.7f, RGBA_RED_COLOR, newBMF, TEXT_DRAW_SHRINK_TO_FIT );
	g_theRenderer->DrawTextInBox2D( std::string("PRESS START FOR MENU"), Vector2(0.5f, 0.0f), boxForText, 
		0.2f, RGBA_RED_COLOR, newBMF, TEXT_DRAW_SHRINK_TO_FIT );
}

void theGame::Render_Victory()
{
	m_theCamera->Render();

	AABB2 boxForText = AABB2( 4.f, 2.25f, 12.f, 6.75f );
	g_theRenderer->DrawTextInBox2D( std::string("Victory..!"), Vector2(0.5f, 0.5f), boxForText, 
		0.7f, RGBA_RED_COLOR, newBMF, TEXT_DRAW_SHRINK_TO_FIT );
	g_theRenderer->DrawTextInBox2D( std::string("PRESS START FOR MENU"), Vector2(0.5f, 0.0f), boxForText, 
		0.2f, RGBA_RED_COLOR, newBMF, TEXT_DRAW_SHRINK_TO_FIT );
}

float theGame::CalculateDeltaTime() {
	double currentTime = GetCurrentTimeSeconds();
	float deltaSeconds = (float)(currentTime - m_lastFramesTime);
	m_lastFramesTime = currentTime;

	if( deltaSeconds > maxSecondsPerFrame ) {
		deltaSeconds = maxSecondsPerFrame;
	}

	return deltaSeconds;
}