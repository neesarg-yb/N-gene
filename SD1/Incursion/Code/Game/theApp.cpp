#include "theApp.hpp"
#include "Game/GameCommon.hpp"


theApp::theApp()
{
	lastFramesTime = GetCurrentTimeSeconds();

	g_theRenderer = new Renderer();
	g_theAudioSystem = new AudioSystem(); 
	g_theGame = new theGame();
	g_theInput = new InputSystem();

}

theApp::~theApp()
{
	delete g_theInput;
	g_theInput = nullptr;

	delete g_theGame;
	g_theGame = nullptr;


	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;

}

void theApp::RunFrame() {
	BeginFrame();		// [ for ENGINE components, generally ]
	Update();			// [ for  GAME  components, generally ]
	Render();			// [ for  GAME  components, generally ]
	EndFrame();			// [ for ENGINE components, generally ]
}

void theApp::BeginFrame() {
	g_theRenderer->BeginFrame();
	g_theAudioSystem->BeginFrame();
	g_theGame->BeginFrame();
	g_theInput->BeginFrame();
}

void theApp::EndFrame() {
	g_theInput->EndFrame();
	g_theGame->EndFrame();
	g_theAudioSystem->EndFrame();
	g_theRenderer->EndFrame();
}

void theApp::Update() {
	// Calculating deltaTime
	float deltaSeconds = CalculateDeltaTime();

	g_theGame->Update( deltaSeconds );
}

void theApp::Render() {
	g_theGame->Render();
}

bool theApp::IsQuitting() {
	return m_isQuitting;
}

void theApp::HandleKeyDown( unsigned char KEY_CODE ) {
	g_theInput->OnKeyPressed(KEY_CODE);
}

void theApp::HandleKeyUp( unsigned char KEY_CODE ) {
	g_theInput->OnKeyReleased(KEY_CODE);
}


float theApp::CalculateDeltaTime() {
	double currentTime = GetCurrentTimeSeconds();
	float deltaSeconds = (float)(currentTime - lastFramesTime);
	lastFramesTime = currentTime;

	if( deltaSeconds > maxSecondsPerFrame ) {
		deltaSeconds = maxSecondsPerFrame;
	}

	return deltaSeconds;
}