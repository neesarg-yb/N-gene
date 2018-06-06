#include "theApp.hpp"


theApp::theApp()
{
	g_theGame = new theGame();
	g_theRenderer = new Renderer();
	g_theInput = new InputSystem();
}

theApp::~theApp()
{

	delete g_theInput;
	g_theInput = nullptr;

	delete g_theRenderer;
	g_theRenderer = nullptr;

	delete g_theGame;
	g_theGame = nullptr;
}

void theApp::BeginFrame() {
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
}

void theApp::EndFrame() {
	g_theRenderer->EndFrame();
	g_theInput->EndFrame();
}

void theApp::Update() {
	g_theGame->Update();
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