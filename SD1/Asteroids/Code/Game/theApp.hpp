#pragma once

#include "theGame.hpp"
#include "Game/GameCommon.hpp"

using namespace std;

class theApp
{
public:
	theGame* g_theGame = nullptr;
	bool m_isQuitting = false;						// ...becomes App::m_isQuitting

	bool IKeyIsPressed = false;
	bool OKeyIsPressed = false;
	bool PKeyIsPressed = false;
	bool TKeyIsPressed = false;
	bool F1KeyIsPressed = false;
	bool SpaceKeyIsPressed = false;
	bool NKeyIsPressed = false;

	void BeginFrame();								// [ ENGINE ]
	void Update();									// [ GAME ]
	void Render();									// [ GAME ]
	void EndFrame();								// [ ENGINE ]
	
	bool IsQuitting();
	void HandleKeyDown( unsigned char KEY_CODE );
	void HandleKeyUp( unsigned char KEY_CODE );

	theApp();
	~theApp();

private:

};
