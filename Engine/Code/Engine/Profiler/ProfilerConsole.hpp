#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"

class Camera;
class Renderer;
class BarGraph;
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
	std::string		m_profileReportString		= "";
	std::string		m_hotkeysInfoString			= "";

private:
	int				 m_maxGraphRecordsNum		= 128;
	int				 m_skipIndex				= 0;	// Skip Index relative to last report in the graph
	AABB2			 m_graphBounds;						// Render_GraphBox() calculates it
	BarGraph		*m_frameGraph				= nullptr;

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

private:
	void	HandleInput( InputSystem& currentInputSystem );
	int		CalculateSkipCountFromLeftOfBox( AABB2 const &boundsOfGraph, Vector2 const &selectionPosition );
};