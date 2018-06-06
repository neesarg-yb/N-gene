#pragma once

#include "Engine/Core/Time.hpp"

using namespace std;

class theApp
{
public:
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
	double lastFramesTime;
	const float minFPS = 20.f;
	const float maxSecondsPerFrame = 1 / minFPS;

	void BeginFrame();								// [ ENGINE ]
	void Update();									// [ GAME ]
	void Render();									// [ GAME ]
	void EndFrame();								// [ ENGINE ]

	float CalculateDeltaTime();
};
