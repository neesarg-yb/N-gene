#pragma once
#include "Engine/Core/Time.hpp"
#include "Engine/Core/UIMenu.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game States/GameState.hpp"

class theGame
{
public:
	 theGame();
	~theGame();

public:
	// UI & Camera
	const AABB2			m_default_screen_bounds	= AABB2( -g_aspectRatio, -1.f, g_aspectRatio, 1.f );
	const Vector3		m_default_screen_center	= Vector3::ZERO;
	const Rgba			m_default_screen_color	= RGBA_BLACK_COLOR;
	BitmapFont*			m_textBmpFont			= nullptr;
	Camera*				m_gameCamera			= nullptr;

	// Game States
	std::vector< GameState* >	 m_gameStates;
	GameState					*m_currentGameState		= nullptr;
	std::string					 m_nextGameStateName	= "NONE";

public:
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Update();
	void Render() const;

	void QuitGame( char const *actionName );

public:
	// Game States
	bool		SetCurrentGameState		( std::string const &gsName );		// returns false if game state of that name doesn't exists
	void		StartTransitionToState	( std::string const &stateName );
	void		AddNewGameState			( GameState* gsToAdd );				// If already exists in list, it replaces with the new one
	GameState*	RemoveGameStateNamed	( std::string const &gsName );		// Doesn't delete, just removes from the list

private:
	// Local
	double			m_timeSinceStartOfTheGame			=	0;
	double			m_lastFramesTime;
	double			m_timeSinceTransitionBegan			=	0;
	const float		m_transitionTime					=	0.5f;
	const float		m_halfTransitionTime				=	m_transitionTime * 0.5f;
	float			m_fadeEffectAlpha					=	0.f;						// 0 to 1
	
private:
	// Game States
	int			FindGameStateNamed( std::string const &stateName, GameState *&outGameState );
	void		ConfirmTransitionToNextState();

	// Loading Screen
	void		RenderLoadingScreen() const;

	// Time
	double		GetTimeSinceGameStarted() const;

	bool		MyTestObjectMethod1( NamedProperties &args );
	bool		MyTestObjectMethod2( NamedProperties &args );
};

bool MyStandaloneFunction( NamedProperties &args );
