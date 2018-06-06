#pragma once

#include "theGame.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Core/Blackboard.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Math/MathUtil.hpp"

class theApp
{
public:
	bool m_isQuitting = false;						// ...becomes App::m_isQuitting

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
