#pragma once
#include "Engine/Core/Time.hpp"
#include "Engine/Core/UIMenu.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Network/RemoteCommandService.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Abstract Classes/GameState.hpp"
#include "Game/Level.hpp"
#include "Game/World/Robot.hpp"

class theGame
{
public:
	 theGame();
	~theGame();

public:
	const AABB2			m_default_screen_bounds		= AABB2( -g_aspectRatio, -1.f, g_aspectRatio, 1.f );
	const Vector3		m_default_screen_center		= Vector3::ZERO;
	const Rgba			m_default_screen_color		= RGBA_BLACK_COLOR;

	BitmapFont*	m_textBmpFont	= nullptr;
	Camera*		m_gameCamera	= nullptr;
	Level*		m_currentLevel	= nullptr;

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

	void StartTransitionToState	( std::string const &stateName );
	void QuitGame				( char const *actionName );

private:
	// Local
	double			m_timeSinceStartOfTheGame			=	0;
	double			m_lastFramesTime;
	double			m_timeSinceTransitionBegan			=	0;
	const float		m_transitionTime					=	2.5;
	const float		m_halfTransitionTime				=	m_transitionTime * 0.5f;
	float			m_fadeEffectAlpha					=	0.f;						// 0 to 1

	
private:
	// Game States
	GameState*	FindGameStateNamed		( std::string const &stateName );
	void		ConfirmTransitionToNextState();

	void Update_Menu	( float deltaSeconds );
	void Render_Menu	() const;

	void RenderLoadingScreen() const;

	float	CalculateDeltaTime();
	double	GetTimeSinceGameStarted() const;
};

