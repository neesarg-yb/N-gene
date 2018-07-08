#pragma once
#include "Engine/Core/Time.hpp"
#include "Engine/Core/UIMenu.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Battle.hpp"

enum GameStates
{
	NONE = -1,
	ATTRACT,
	MENU,
	BATTLE,
	VICTORY,
	NUM_GAME_STATES
};

class theGame
{
public:
	 theGame();
	~theGame();

	const AABB2			m_default_screen_bounds		= AABB2( -g_aspectRatio, -1.f, g_aspectRatio, 1.f );
	const Vector3		m_default_screen_center		= Vector3::ZERO;
	const Rgba			m_default_screen_color		= RGBA_BLACK_COLOR;

	BitmapFont*	m_textBmpFont	= nullptr;
	Camera*		m_gameCamera	= nullptr;
	Battle*		m_currentBattle	= nullptr;

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render() const;

private:
	double			m_timeSinceStartOfTheGame	=	0;
	double			m_lastFramesTime;
	double			m_timeSinceTransitionBegan	=	0;
	const float		m_transitionTime			=	2.5;
	const float		m_halfTransitionTime		=	m_transitionTime * 0.5f;
	float			m_fadeEffectAlpha			=	0.f;						// 0 to 1
	GameStates		m_currentGameState			=	ATTRACT;
	GameStates		m_nextGameState				=	NONE;

	// Audio
	SoundID			m_attractMusic;
	SoundID			m_anticipateMusic;
	SoundID			m_battleBackgroundMusic;
	SoundPlaybackID	m_attractPlayback;
	SoundPlaybackID	m_anticipatePlayback;
	SoundPlaybackID	m_battleBackgroundPlayback;

	// Menu Specific
	UIMenu*								 m_attractMenu	= nullptr;
	std::function< void( const char* ) > quitStdFunc	= std::bind( &theGame::QuitGame,	  this, std::placeholders::_1 );
	std::function< void( const char* ) > startStdFunc	= std::bind( &theGame::GoToMenuState, this, std::placeholders::_1 );
	
	void GoToMenuState( char const *actionName );
	void QuitGame( char const *actionName );

	// Game States
	void StartTransitionToState	( GameStates nextGameState );
	void ConfirmTransitionToNextState();

	void Update_Attract	( float deltaSeconds );
	void Update_Menu	( float deltaSeconds );
	void Update_Battle	( float deltaSeconds );
	void Update_Victory	( float deltaSeconds );

	void Render_Attract	() const;
	void Render_Menu	() const;
	void Render_Battle	() const;
	void Render_Victory	() const;

	void RenderLoadingScreen() const;

	float	CalculateDeltaTime();
	double	GetTimeSinceGameStarted() const;
};
