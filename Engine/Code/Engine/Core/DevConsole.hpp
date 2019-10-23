#pragma once
#include <functional>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/LogSystem/SpinLock.hpp"

class Command;

typedef std::function< void( char const *) > devConsoleHook_cb;

struct OutputStringsBuffer
{
	std::string	m_line_str;
	Rgba		m_line_color;

	OutputStringsBuffer( std::string line_str, Rgba line_color = RGBA_WHITE_COLOR )
	{
		m_line_str		= line_str;
		m_line_color	= line_color;
	}
};

// Will be a singleton
class DevConsole 
{
private:
	 DevConsole( Renderer* currentRenderer ); 
	~DevConsole();

public:
	// Handles all input
	void Update( InputSystem& currentInputSystem );

	// Renders the display
	void Render(); 

	void Open(); 
	void Close(); 
	bool IsOpen(); 
	void ClearOutputBuffer();

	// Console Hook
	void DevConsoleHook		( devConsoleHook_cb *cbToHook );
	void DevConsoleUnhook	( devConsoleHook_cb *cbToUnhook );

private:
			float	CalculateDeltaTime();
			void	ClearInputBuffer();
			void	DrawBlinker();
	static	size_t	GetBlinkerPosition();
			void	KeyActions_HandleLeftArrowKey( float deltaSeconds );
			void	KeyActions_HandleRightArrowKey( float deltaSeconds );
			void	KeyActions_HandleUpArrowKey();
			void	KeyActions_HandleDownArroyKey();
			bool	KeyActions_HandleBackspace( float deltaSeconds );
			void	KeyActions_HandleDeleteKey( float deltaSeconds );
			void	KeyActions_HandleEscapeKey();
			void	KeyActions_HandleEnterKey ();
			void	PrintTheOutputBuffer( int scrollAmount = 0 );
	static	void	ResetTheScroll( Command& cmd );

	// Command History
	inline	void	ResetHistorySkipCount() { m_historySkipCount = -1; }
			void	IncrementHistorySkipCountBy( int increment );
			void	AddStringToTheCommandHistory( std::string const &commandStr );

private: 
	double lastFramesTime;

	static	bool		s_isOpen;
	static	int			s_scrollAmount;
	static	Camera*		s_devConsoleCamera;
	static	DevConsole*	s_devConsoleInstance;
			Renderer*	m_currentRenderer;
			BitmapFont* m_fonts;
	const	Vector2		m_bottomLeftOrtho			= Vector2( -g_aspectRatio, -1.f );			//		Treat like a,
	const	Vector2		m_topRightOrtho				= Vector2(  g_aspectRatio,  1.f );			//					 NDC
	const	AABB2		m_consoleBackgroundBox		= AABB2( m_bottomLeftOrtho, m_topRightOrtho );
	const	AABB2		m_inputAreaBox				= AABB2( m_bottomLeftOrtho, Vector2( m_topRightOrtho.x, m_bottomLeftOrtho.y + 0.06f ) );
	const	Rgba		m_consoleBackgroundColor	= Rgba( 0, 0, 0, 200 );
	const	AABB2		m_inputAreaTextBox			= AABB2( m_inputAreaBox.mins + Vector2( 0.01f, 0.f ), m_inputAreaBox.maxs - Vector2( 0.01f, 0.f ) ); 
	const	AABB2		m_outputAreaTextBox			= AABB2( Vector2( m_inputAreaTextBox.mins.x, m_inputAreaBox.maxs.y ) + Vector2( 0.01f, 0.f ), m_topRightOrtho - Vector2( 0.01f, 0.f ) );
	const	float		m_textHeight				= ( m_inputAreaBox.maxs.y - m_inputAreaBox.mins.y ) * 0.5f;

	static	float		s_blinkerMoveSpeed;			// In Characters Per Seconds. ( It is used when Backspace, Delete, and Arrow keys are pressed.. )
	static	int			s_blinkerPosition;
	static	bool		s_blinkerHidden;
	static	std::string	s_inputBufferString;
	static	SpinLock	s_outputBufferLock;
	static	std::vector< OutputStringsBuffer >	s_outputBuffer;

			int							m_historySkipCount	= -1;
	const	int							m_maxHistoryCount	= 50;
	std::vector< std::string >			m_commandHistory;
	std::vector< devConsoleHook_cb* >	m_consoleHooks;

public:
	static DevConsole*	InitalizeSingleton( Renderer& currentRenderer );
	static void			DestroySingleton();
	static DevConsole*	GetInstance();
	static bool			ConsoleMessageHandler( unsigned int wmMessageCode, size_t wParam, size_t lParam );
	static void			WriteToOutputBuffer( std::string line_str, Rgba line_color = RGBA_WHITE_COLOR );
	
	static std::vector< std::string >	GetOutputBufferLines();
};


// Global functions

// I like this as a C function or a static 
// method so that it is easy for systems to check if the dev.
// console is open.  
bool DevConsoleIsOpen(); 

// Should add a line of colored text to the output 
void ConsolePrintf( Rgba const &color, char const *format, ... ); 

// Same as previous, be defaults to a color visible easily on your console
void ConsolePrintf( char const *format, ... ); 
