#pragma once
#include "Game/Polygon2.hpp"
#include "Game/Map.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/Camera2D.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class theGame
{
public:
	bool showAttractScreen = true;
	bool canQuitApp = false;
	bool screenShakeEnabled = false;
	bool lockEscKey = false;

	SoundID attractSound;
	SoundPlaybackID attractSoundPlaybackID;
	
	SoundID gamePausedSound;
	SoundID gameUnPausedSound;

	bool showDeathScreen = false;
	bool gameIsPaused = false;
	bool slowMoIsOn   = false;
	bool fastMoIsOn   = false;
	
	theGame();
	~theGame();
	Map *currentMap;
	PlayerTank *playerTank;
	Camera2D *theCamera;
	
	const Vector2 default_ortho_leftBottom = Vector2( 0.f, 0.f );
	const Vector2 default_ortho_topRight = Vector2( 30.f, 30.f );
	const Rgba	  default_screen_color = Rgba( 0, 0, 0, 255 );


	IntVector2 mapGridSize = IntVector2( (int) default_ortho_topRight.x , (int) default_ortho_topRight.y );

	void BeginFrame();
	void Update( float deltaSeconds );
	void Render();
	void EndFrame();

private:
	void  Reflect_GameInterface_Related_InputChanges();


	float deathFadeTimer = 3.0f;
	float fade = 0;
	float timeElaspedAfterDeath = 0.0f;
};

