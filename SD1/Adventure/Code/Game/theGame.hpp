#pragma once
#include "ThirdParty/tinyxml/tinyxml2.h"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/XMLUtilities.hpp"
#include "Engine/Renderer/SpriteAnimSetDefinition.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/Map.hpp"
#include "Game/Camera2D.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/ProjectileDefinition.hpp"
#include "Game/PortalDefinition.hpp"
#include "Game/Player.hpp"
#include "Game/AdventureDefinition.hpp"
#include "Game/Adventure.hpp"

using namespace tinyxml2;

enum GameState
{
	NONE = -1,
	ATTRACT,
	PLAYING,
	PAUSED,
	DEFEAT,
	VICTORY,
	NUM_GAME_STATES
};

class theGame
{
public:
	theGame();
	~theGame();

	const float minFPS = 20.f;
	const float maxSecondsPerFrame = 1 / minFPS;

	Adventure*		m_currentAdventure			= nullptr;
	Player*			m_potentialPlayer			= nullptr;
	Camera2D*		m_theCamera					= nullptr;
	BitmapFont*		newBMF						= nullptr;

	const Vector2	m_default_ortho_leftBottom	= Vector2( 0.f, 0.f );
	const Vector2	m_default_ortho_topRight	= Vector2( 16.f, 9.f );
	const Rgba		m_default_screen_color		= Rgba( 0, 0, 0, 255 );
	const float		m_default_overlay_time		= 1.f;

	GameState		m_currentState				= ATTRACT;
	GameState		m_transitionToState			= NONE;
	bool			m_isFinishedTransitioning	= true;
	float			m_secondsInCurrentState		= 0.f;
	float			m_secondsInStateTransition	= 0.f;

	void BeginFrame();
	void Update();
	void Render();
	void EndFrame();

	void StartTransitionToState( GameState nextGameState );

	void Update_Attract( float deltaSeconds );
	void Update_Playing( float deltaSeconds );
	void Update_Paused ( float deltaSeconds );
	void Update_Defeat ( float deltaSeconds );
	void Update_Victory( float deltaSeconds );

	void Render_Attract();
	void Render_Playing();
	void Render_Paused ();
	void Render_Defeat ();
	void Render_Victory();

private:
	double m_lastFramesTime;
	
	float CalculateDeltaTime();
};

