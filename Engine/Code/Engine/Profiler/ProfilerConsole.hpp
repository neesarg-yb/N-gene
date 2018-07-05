#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"

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
	AABB2 const		m_drawBounds				= AABB2( -g_aspectRatio, -1.f, g_aspectRatio, 1.f );

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