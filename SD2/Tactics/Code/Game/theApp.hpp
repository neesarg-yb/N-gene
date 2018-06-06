#pragma once

#include "theGame.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/../ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Core/Blackboard.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/DevConsole.hpp"

using namespace tinyxml2;

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

	void RunFrame();

	bool IsQuitting();
	void HandleKeyDown( unsigned char KEY_CODE );
	void HandleKeyUp( unsigned char KEY_CODE );

	 theApp();
	~theApp();

private:

	void BeginFrame();								// [ ENGINE ]
	void Update();									// [ GAME ]
	void Render();									// [ GAME ]
	void EndFrame();								// [ ENGINE ]
};
