#include "theGame.hpp"
#include "Game/GameCommon.hpp"


theGame::theGame()
{
	MathUtil::NewSeedForRandom();

	playerTank = new PlayerTank();
	currentMap = new Map( mapGridSize, playerTank );
	theCamera = new Camera2D(12.f);

	attractSound = g_theAudioSystem->CreateOrGetSound( "Data/Audio/AttractMusic.mp3" );
	attractSoundPlaybackID = g_theAudioSystem->PlaySound( attractSound );
	g_theAudioSystem->SetSoundPlaybackVolume( attractSound , 0.5f );

	gamePausedSound = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Pause.mp3" );
	gameUnPausedSound = g_theAudioSystem->CreateOrGetSound( "Data/Audio/Unpause.mp3" );
}

theGame::~theGame()
{
	delete theCamera;
	delete currentMap;
	delete playerTank;
}

void theGame::BeginFrame() {
	if( showDeathScreen )
		return;

	currentMap->BeginFrame();
}

void theGame::EndFrame() {
	if( showDeathScreen )
		return;

	currentMap->EndFrame();
}

void theGame::Update( float deltaSeconds) {

	Reflect_GameInterface_Related_InputChanges();

	if( screenShakeEnabled )
	{
		float maxShakeTime = 0.35f;
		// If player is dead, longer screenshake..
		if( playerTank->m_health <= 0 )
		{
			maxShakeTime = 1.5f;
		}

		static float screenShakeTimeElasped = 0.f;

		// TimeElasped is more than maxShakeTime seconds
		if( screenShakeTimeElasped >= maxShakeTime)
		{
			screenShakeEnabled = false;
			screenShakeTimeElasped = 0.f;
		}

		screenShakeTimeElasped += deltaSeconds;
	}

	if( showAttractScreen == true )
	{

		if( g_theInput->WasKeyJustPressed(VK_Codes::SPACE) || g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed )
		{
			showAttractScreen = false;
			g_theAudioSystem->StopSound(attractSoundPlaybackID);
		}

		return;
	}

	if( showDeathScreen == true )
	{
		if( g_theInput->WasKeyJustPressed(VK_Codes::P) || g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed )
		{
			showDeathScreen = false;
			currentMap->RespawnPlayerTank();
		}
	}

	if( playerTank->m_health <= 0)
	{
		timeElaspedAfterDeath += deltaSeconds;

		deltaSeconds *= 0.1f;

		// Player is dead, increase fading..
		if( !showDeathScreen )
		{
			float fadePerSecond = 500.0f;			// 500.0 doesn't look like a correct number, but it works.
			fade += fadePerSecond * deltaSeconds;
		}
		else
		{
			fade = 0.0f;
		}

		if( timeElaspedAfterDeath >= deathFadeTimer )
		{

			timeElaspedAfterDeath = 0.0f;
			showDeathScreen = true;
			
		}
	}

	if(gameIsPaused) {
		deltaSeconds *= 0.f;
	}
	if(slowMoIsOn) {
		deltaSeconds *= 0.1f;
	}
	if(fastMoIsOn) {
		deltaSeconds *= 2.f;
	}

	currentMap->Update(deltaSeconds);

}

void theGame::Render() {
	// Establish a 2D (orthographic) drawing coordinate system
	// g_theRenderer->SetOrtho( default_ortho_leftBottom, default_ortho_topRight );
	float randomShakeDisplacement = 0.f;
	Vector2 camearaPos = playerTank->GetPosition();

	if( screenShakeEnabled )
	{
		randomShakeDisplacement = MathUtil::GetRandomFloatInRange( 0.f, 0.05f );
		 camearaPos += Vector2( randomShakeDisplacement, 0.f);
	}

	theCamera->MoveCameraTo( camearaPos );

	// Clear all screen (backbuffer) pixels to default_screen_color
	g_theRenderer->ClearScreen(default_screen_color);

	
	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  Game States: (before map's rendering)
	//
	if( showAttractScreen == true )
	{
		Texture* attractScreen = g_theRenderer->CreateOrGetTexture("Data//Images//AttractScreen.png");
		theCamera->OverlayTextureOnScreen(*attractScreen);

		return;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  Start drawing from here,
	//
	currentMap->Render();


	/////////////////////////////////////////////////////////////////////////////////////////////
	//
	//  Game States: (on top of map's rendering)
	//

	if(gameIsPaused)
	{
		Texture* pauseScreen = g_theRenderer->CreateOrGetTexture("Data//Images//PauseScreen.png");

		// TODO: Alpha is set to 200 in this function, for some reason it doesn't work with png having alpha != 100%
		theCamera->OverlayTextureOnScreen(*pauseScreen);
	}

	if(showDeathScreen)
	{
		Texture* deathScreen = g_theRenderer->CreateOrGetTexture("Data//Images//DiedScreen.png");

		// TODO: Alpha is set to 200 in this function, for some reason it doesn't work with png having alpha != 100%
		theCamera->OverlayTextureOnScreen(*deathScreen);
	}

	if( playerTank->m_health <= 0 )
	{
		// Fade the screen
		if( showDeathScreen != true )
		{
			lockEscKey = true;
			theCamera->OverlaySolidAABBOnScreenWithAlpha( RGBA_BLACK_COLOR , fade );
		}
		else
		{
			lockEscKey = false;
		}
	}
}


void theGame::Reflect_GameInterface_Related_InputChanges() {

	if( g_theInput->IsKeyPressed( VK_Codes::T ) ) {
		slowMoIsOn = true;
	} else {
		slowMoIsOn = false;
	}
	if( g_theInput->IsKeyPressed( VK_Codes::F ) ) {
		fastMoIsOn = true;
	} else {
		fastMoIsOn = false;
	}

	if( g_theInput->WasKeyJustPressed( VK_Codes::F1 ) ) {
		currentMap->debugModeOn = !currentMap->debugModeOn;
	}
	if( g_theInput->WasKeyJustPressed( VK_Codes::P ) || g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed ) {
		if( !playerTank->isInvisible && !showDeathScreen && !showAttractScreen )
			gameIsPaused = !gameIsPaused;

		if( gameIsPaused ) {
			g_theAudioSystem->PlaySound(gamePausedSound);
		}
		else {
			g_theAudioSystem->PlaySound(gameUnPausedSound);
		}
	}
	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE) )
	{
		if( showAttractScreen == true )
		{
			canQuitApp = true;
		}

		if( gameIsPaused == true )
		{
			showAttractScreen = true;
			attractSoundPlaybackID = g_theAudioSystem->PlaySound( attractSound );
		}

		if( gameIsPaused == false && showDeathScreen == false && lockEscKey == false )
		{
			gameIsPaused = true;
		}

		if( showDeathScreen == true )
		{
			showDeathScreen = false;
			currentMap->RespawnPlayerTank();
			showAttractScreen = true;
		}
		// canQuitApp = true;
	}
	if( g_theInput->WasKeyJustPressed( VK_Codes::I ) )
	{
		static bool newOrthoSet = false;

		int newOrtho;
		if( !newOrthoSet )
		{
			newOrtho = currentMap->gridSize.x > currentMap->gridSize.y ? currentMap->gridSize.x : currentMap->gridSize.y;
			newOrthoSet = true;
		}
		else
		{
			newOrtho = 12;
			newOrthoSet = false;
		}

		delete theCamera;
		theCamera = new Camera2D((float)newOrtho);
	}
	if( g_theInput->WasKeyJustPressed( VK_Codes::O ) )
	{
		currentMap->zeroModeEnable = !currentMap->zeroModeEnable;
	}
}