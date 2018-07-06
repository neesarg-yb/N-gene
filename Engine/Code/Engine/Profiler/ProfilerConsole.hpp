#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"

class Camera;
class Renderer;
class BitmapFont;
class InputSystem;

class ProfileConsole
{
public:
	 ProfileConsole( Renderer* currentRenderer );
	~ProfileConsole();

private:
	static	ProfileConsole*	s_profileConsoleInstance;

public:
	static	ProfileConsole* GetInstance();

public:
	bool			m_isOpen					= false;
	Camera*			m_profileConsoleCamera		= nullptr;
	Renderer*		m_currentRenderer			= nullptr;
	BitmapFont*		m_fonts						= nullptr;
	AABB2	const	m_drawBounds				= AABB2( -g_aspectRatio, -1.f, g_aspectRatio, 1.f );
	Rgba	const	m_accentColor				= Rgba( 70, 141, 185, 200 );
	Rgba	const	m_boxBackgroudColor			= Rgba( 0, 0, 0, 180 );

	int				m_fps						= 0;
	double			m_frameTime					= 0;
	uint64_t		m_lastFramesHPC				= 0;
	std::string		m_profileReportString		= "";
	std::string		m_hotkeysInfoString			= "";

public:
	void Update( InputSystem& currentInputSystem );
	void Render();

	void Open();
	void Close();
	bool IsOpen();

private:
	void Render_Backgroud();
	void Render_FPSBox();
	void Render_HotkeysBox();
	void Render_GraphBox();
	void Render_ProfilingDetailsBox();
};