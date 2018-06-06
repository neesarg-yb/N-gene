#pragma once

#include "theApp.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"


theApp::theApp()
{
	g_theRenderer = new Renderer();
	g_theGame = new theGame();
	g_theInput = new InputSystem();

	NewSeedForRandom();
}

theApp::~theApp()
{

	delete g_theInput;
	g_theInput = nullptr;

	delete g_theGame;
	g_theGame = nullptr;

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
	g_theInput->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theGame->BeginFrame();
}

void theApp::EndFrame() {
	g_theGame->EndFrame();
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