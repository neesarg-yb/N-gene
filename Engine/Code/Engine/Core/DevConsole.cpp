#pragma once
#include <fstream>
#include "DevConsole.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/LogSystem/LogSystem.hpp"

int				DevConsole::s_scrollAmount			= 0;
Camera*			DevConsole::s_devConsoleCamera		= nullptr;
DevConsole*		DevConsole::s_devConsoleInstance	= nullptr;
bool			DevConsole::s_isOpen				= false;
int				DevConsole::s_blinkerPosition		= 0;
bool			DevConsole::s_blinkerHidden			= false;
std::string		DevConsole::s_inputBufferString		= "";

SpinLock							DevConsole::s_outputBufferLock;
std::vector< OutputStringsBuffer>	DevConsole::s_outputBuffer;

void echo_with_color	( Command& cmd );
void clear_console		( Command& cmd );
void save_log_to_file	( Command& cmd );
void print_all_registered_commands	( Command& cmd );
void HookDevConsoleToLogSystem( Command &cmd );
void UnhookDevConsoleToLogSystem( Command &cmd );

void LogHookWritesToConsole( LogData const *logData, void * )
{
	std::string logStr = Stringf( "%s: %s", logData->tag.c_str(), logData->text.c_str() );
	DevConsole::GetInstance()->WriteToOutputBuffer( logStr, RGBA_GRAY_COLOR );
}

DevConsole::DevConsole( Renderer* currentRenderer )
	: m_currentRenderer( currentRenderer )
{
	GUARANTEE_OR_DIE( m_currentRenderer != nullptr, "Renderer passed to DevConsole can't be a nullptr!" );

	lastFramesTime = GetCurrentTimeSeconds();
	m_fonts = currentRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");

	CommandRegister( "echo_with_color", echo_with_color );
	CommandRegister( "clear", clear_console );
	CommandRegister( "save_log", save_log_to_file );
	CommandRegister( "help", print_all_registered_commands );
	CommandRegister( "scroll_bottom", DevConsole::ResetTheScroll );
	CommandRegister( "log_hook_devconsole", HookDevConsoleToLogSystem );
	CommandRegister( "log_unhook_devconsole", UnhookDevConsoleToLogSystem );
}

DevConsole::~DevConsole()
{

}

void DevConsole::Update( InputSystem& currentInputSystem )
{
	// Calculating deltaTime
	float deltaSeconds = CalculateDeltaTime();

	// Trigger blinker
	static float blinkAfter = .5f;
	blinkAfter -= deltaSeconds;

	if( blinkAfter <= 0 )
	{
		s_blinkerHidden = !s_blinkerHidden; 
		blinkAfter = .5f;
	}


	if( currentInputSystem.WasKeyJustPressed(BACK) )
		KeyActions_HandleBackspace();
	if( currentInputSystem.WasKeyJustPressed(LEFT) )
		KeyActions_HandleLeftArrowKey();
	if( currentInputSystem.WasKeyJustPressed(RIGHT) )
		KeyActions_HandleRightArrowKey();
	if( currentInputSystem.WasKeyJustPressed(DELETE_KEY) )
		KeyActions_HandleDeleteKey();
	if( currentInputSystem.WasKeyJustPressed(ESCAPE) )
		KeyActions_HandleEscapeKey();
	if( currentInputSystem.WasKeyJustPressed(ENTER) )
		KeyActions_HandleEnterKey();
}

void DevConsole::Render()
{
	m_currentRenderer->BindCamera( s_devConsoleCamera );
	
	// To form an overlay: do not clear screen, make depth of every pixel 1.f, do not write new depth..
	m_currentRenderer->UseShader( nullptr );
	m_currentRenderer->EnableDepth( COMPARE_ALWAYS, false );
	
	// Draw overlay
	m_currentRenderer->DrawAABB( m_consoleBackgroundBox, m_consoleBackgroundColor );
	// Draw input area
	m_currentRenderer->DrawAABB( m_inputAreaBox, RGBA_GRAY_COLOR );
	// Draw the input buffer
	if( s_inputBufferString != "" )
		m_currentRenderer->DrawTextInBox2D( s_inputBufferString,  Vector2( 0.f, 0.5f ) , m_inputAreaTextBox, m_textHeight, RGBA_BLACK_COLOR, m_fonts, TEXT_DRAW_OVERRUN );
	// Draw the output buffer
	PrintTheOutputBuffer( s_scrollAmount );

	DrawBlinker();
}

void DevConsole::Open()
{
	s_isOpen = true;
}

void DevConsole::Close()
{
	s_isOpen = false;
}

bool DevConsole::IsOpen()
{
	return s_isOpen;
}

void DevConsole::ClearOutputBuffer()
{
	s_outputBufferLock.Enter();
	s_outputBuffer.clear();
	s_outputBufferLock.Leave();
}

DevConsole* DevConsole::InitalizeSingleton( Renderer& currentRenderer )
{
	if(s_devConsoleInstance == nullptr)
		s_devConsoleInstance = new DevConsole( &currentRenderer );
	else
		GUARANTEE_RECOVERABLE( false, "Note: One instance if DevConsole exist, can't create another one!" );
	
	if( s_devConsoleCamera != nullptr )
		delete s_devConsoleCamera;
	s_devConsoleCamera = new Camera();

	// Setting up the Camera
	s_devConsoleCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	s_devConsoleCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	s_devConsoleCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );			// Make an NDC

	return s_devConsoleInstance;
}

void DevConsole::DestroySingleton()
{
	delete s_devConsoleCamera;
	s_devConsoleCamera = nullptr;

	delete s_devConsoleInstance;
	s_devConsoleInstance = nullptr;
}

DevConsole* DevConsole::GetInstance()
{
	return s_devConsoleInstance;
}

float DevConsole::CalculateDeltaTime() {
	double currentTime = GetCurrentTimeSeconds();
	float deltaSeconds = (float)(currentTime - lastFramesTime);
	lastFramesTime = currentTime;

	return deltaSeconds;
}

bool DevConsole::ConsoleMessageHandler( unsigned int wmMessageCode, size_t wParam, size_t lParam )
{
	// If Console is not open, inputDisabled should be set true
	if( !s_isOpen )
		return true;

	UNUSED( lParam );

	switch( wmMessageCode )
	{
		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_CHAR:
		{
			unsigned char asKey = (unsigned char) wParam;

			// Ignoring tilde,  backspace,		 enter,			  escape			here..
			if( asKey == '`' || asKey == '\b' || asKey == '\r' || asKey == '\x1b' )
				break;

			std::string asKeyStr;											// To safely convert,
			asKeyStr.push_back( asKey );									//		unsigned char to std::string
			s_inputBufferString.insert( GetBlinkerPosition(), asKeyStr );	
			s_blinkerPosition++;
			break;
		}

		case WM_MOUSEWHEEL:
		{
			short zDelta = (short) HIWORD(wParam);
			s_scrollAmount += (int)(zDelta / 120);

			s_scrollAmount = s_scrollAmount < 0 ? 0 : s_scrollAmount;

			break;
		}

		case  WM_RBUTTONUP:
		{
			s_scrollAmount = 0;
			break;
		}
	}

	return true; 
}

void DevConsole::ClearInputBuffer()
{
	s_inputBufferString = ""; 
	s_blinkerPosition = 0;
}

void DevConsole::DrawBlinker()
{
	if( !s_blinkerHidden )
		m_currentRenderer->DrawText2D( m_bottomLeftOrtho + Vector2( s_blinkerPosition*m_textHeight , 0.01f ), "|", m_textHeight, RGBA_RED_COLOR, m_fonts );
}

size_t DevConsole::GetBlinkerPosition()
{
	s_blinkerPosition = s_blinkerPosition > (int)s_inputBufferString.length() ? (int)s_inputBufferString.length() : s_blinkerPosition;
	s_blinkerPosition = s_blinkerPosition < 0 ? 0 : s_blinkerPosition;

	return static_cast< size_t >( s_blinkerPosition );
}

void DevConsole::KeyActions_HandleBackspace()
{
	if( s_inputBufferString.size() != 0 )
	{
		s_blinkerPosition--;
		s_inputBufferString.erase( GetBlinkerPosition(), 1 );
	}
}

void DevConsole::KeyActions_HandleLeftArrowKey()
{
	if( s_blinkerPosition > 0 )
		s_blinkerPosition--;
}

void DevConsole::KeyActions_HandleRightArrowKey()
{
	if( static_cast<unsigned int>(s_blinkerPosition) < s_inputBufferString.length() )
		s_blinkerPosition++;
}

void DevConsole::KeyActions_HandleDeleteKey()
{
	s_blinkerPosition++;
	KeyActions_HandleBackspace();
}

void DevConsole::KeyActions_HandleEscapeKey()
{
	if( s_inputBufferString.length() != 0 )
		ClearInputBuffer();
	else
		Close();
}

void DevConsole::KeyActions_HandleEnterKey()
{
	// Add the command to outputBufferString
	if( s_inputBufferString != "" )
		WriteToOutputBuffer( "Command: " + s_inputBufferString );
	else
		WriteToOutputBuffer( "Command:" );

	// Execute command..
		// If failed, print command failed
	bool executionSucceeded = false;
	if(s_inputBufferString != "")
		executionSucceeded = CommandRun( s_inputBufferString.c_str() );

	if( executionSucceeded == false )
	{
		WriteToOutputBuffer( "Invalid Command!", RGBA_RED_COLOR );
	}

	// Clear the InputBuffer
	ClearInputBuffer();
}

void DevConsole::WriteToOutputBuffer( std::string line_str, Rgba line_color /*= RGBA_WHITE_COLOR*/ )
{
	if( line_str != "" )
	{
		s_outputBufferLock.Enter();
		s_outputBuffer.push_back( OutputStringsBuffer(line_str, line_color) );
		s_outputBufferLock.Leave();
	}
}

std::vector< std::string > DevConsole::GetOutputBufferLines()
{
	std::vector< std::string > toReturn;
	
	s_outputBufferLock.Enter();
	for each (OutputStringsBuffer op_line in s_outputBuffer )
	{
		toReturn.push_back( op_line.m_line_str );
	}
	s_outputBufferLock.Leave();

	return toReturn;
}

void DevConsole::PrintTheOutputBuffer( int scrollAmount /* = 0 */ )
{
	s_outputBufferLock.Enter();
	for( size_t opIndex = 0; opIndex < s_outputBuffer.size(); opIndex++ )
	{
		Vector2 drawMins = m_outputAreaTextBox.mins + Vector2( 0.f, m_textHeight * (1 + scrollAmount) * ( s_outputBuffer.size() - opIndex ) );
		m_currentRenderer->DrawText2D( drawMins, s_outputBuffer[opIndex].m_line_str, m_textHeight, s_outputBuffer[opIndex].m_line_color, m_fonts );
	}
	s_outputBufferLock.Leave();
}

void DevConsole::ResetTheScroll( Command& cmd )
{
	UNUSED( cmd );
	s_scrollAmount = 0;
}

bool DevConsoleIsOpen()
{
	return false;
}

void ConsolePrintf( Rgba const &color, char const *format, ... )
{
	va_list args;

	va_start( args, format );
	std::string buffer = Stringv( format, args );
	va_end( args );

	DevConsole::GetInstance()->WriteToOutputBuffer( buffer.c_str(), color );
}

void ConsolePrintf( char const *format, ... )
{
	va_list args;
	va_start( args, format );

	char buffer[1000];
	vsnprintf_s( buffer, 1000, format, args );

	va_end( args );
	DevConsole::GetInstance()->WriteToOutputBuffer( buffer );
}

void echo_with_color( Command& cmd )
{
	Rgba color				= cmd.GetNextColor();
	std::string printStr	= cmd.GetNextString();

	if( printStr != "" )
		ConsolePrintf( color, printStr.c_str() );
	else
		ConsolePrintf( color, "Error: Nothing to print.." );
}

void clear_console( Command& cmd )
{
	UNUSED( cmd );
	DevConsole::GetInstance()->ClearOutputBuffer();
}

void save_log_to_file( Command& cmd )
{
	UNUSED( cmd );

	std::string toWrite;
	std::vector< std::string > outputBuffLines = DevConsole::GetOutputBufferLines();

	for each (std::string outputBufferLine in outputBuffLines)
	{
		toWrite += outputBufferLine;
		toWrite += "\n";
	}

	std::ofstream fileWriter("DevConsoleOutput.txt");
	
	if( fileWriter.is_open() )
	{
		fileWriter << toWrite;
		ConsolePrintf( RGBA_GREEN_COLOR, "Log saved.");
	}
	else
		ConsolePrintf( RGBA_RED_COLOR, "Error: Writing in a file, failed!");
	
	fileWriter.close();
}

void print_all_registered_commands( Command& cmd )
{
	UNUSED( cmd );

	std::vector< std::string > commands = GetAllRegisteredCommands();

	ConsolePrintf( "Registered commands:" );
	for( size_t i=0; i<commands.size(); i++ )
	{
		std::string lineStr;
		lineStr += " (";
		lineStr += std::to_string( i );
		lineStr += ") ";
		lineStr += commands[i];

		ConsolePrintf( RGBA_GRAY_COLOR, lineStr.c_str() );
	}
}

void HookDevConsoleToLogSystem( Command &cmd )
{
	UNUSED( cmd );
	LogSystem::GetInstance()->LogHook( LogHookWritesToConsole, nullptr );
}

void UnhookDevConsoleToLogSystem( Command &cmd )
{
	UNUSED( cmd );
	LogSystem::GetInstance()->LogUnhook( LogHookWritesToConsole, nullptr );
}