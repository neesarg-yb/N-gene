#pragma once
#include "Engine/Core/Time.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "ThirdParty/tinyxml/tinyxml2.h"
#include "Game/Polygon2.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Battle.hpp"

using namespace tinyxml2;

enum GameStates
{
	NONE = -1,
	LOADING,
	MENU,
	BATTLE,
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
	
	ShaderProgram* m_grayScaleShaderProgram = nullptr;

	void Update();
	void Render() const;

private:
	double			m_timeSinceStartOfTheGame	=	0;
	double			m_lastFramesTime;
	double			m_timeSinceTransitionBegan	=	0;
	const float		m_transitionTime			=	2.5;
	const float		m_halfTransitionTime		=	m_transitionTime * 0.5f;
	float			m_fadeEffectAlpha			=	0.f;						// 0 to 1
	GameStates		m_currentGameState			=	LOADING;
	GameStates		m_nextGameState				=	NONE;

	void StartTransitionToState	( GameStates nextGameState );
	void ConfirmTransitionToNextState();

	void Update_Loading	( float deltaSeconds );
	void Update_Menu	( float deltaSeconds );
	void Update_Battle	( float deltaSeconds );

	void Render_Loading	() const;
	void Render_Menu	() const;
	void Render_Battle	() const;

	float	CalculateDeltaTime();
	double	GetTimeSinceGameStarted() const;
};

