#pragma once
#include "LogSystem.hpp"

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif

#include <thread>
#include <iostream>

#include "Engine/Internal/WindowsCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/File/File.hpp"
#include "Engine/Math/MathUtil.hpp"

LogSystem	*g_logSystem	= nullptr;
File		*g_logFile		= nullptr;

void WriteToFile( LogData const *logData, void *filePointer )
{
	File *fp = (File*) filePointer;
	
	std::string logStr;
	LogSystem::GetAsHTMLTagFromLogData( logStr, logData );

	fp->Write( logStr );
}

void WriteToIDE( LogData const *logData, void* )
{
	// Get log message as string
	std::string messageStr;
	LogSystem::GetAsStringFromLogData( messageStr, logData );

	// If it is windows, print of its custom debug output
#if defined( PLATFORM_WINDOWS )
	if( IsDebuggerAvailable() )
	{
		OutputDebugStringA( messageStr.c_str() );
	}
#endif

	// Print as normal cout, as well
	std::cout << messageStr;
}

LogSystem::LogSystem()
{

}

LogSystem::~LogSystem()
{

}

LogSystem* LogSystem::GetInstance()
{
	if( g_logSystem == nullptr )
		g_logSystem = new LogSystem();

	return g_logSystem;
}

void LogSystem::LoggerStartup( char const *fileRootName /*= DEFAULT_LOG_NAME */ )
{
	m_isRunning = true;
	
	// Get ready the log file
	g_logFile			= new File();
	bool fileGotOpened	= g_logFile->Open( fileRootName, FILE_OPEN_MODE_TRUNCATE );

	// Hook it if got opened successfully
	if( fileGotOpened )
	{
		// Write HTML heading tags
		std::string htmlUptoBodyStr = Stringf( "<html>\n<head>\n<title> %s </title>\n</head>\n\n<body>\n", fileRootName );
		g_logFile->Write( htmlUptoBodyStr );

		// Set it as a Hook
		LogHook( WriteToFile, g_logFile );
	}
	else
	{
		delete g_logFile;
		g_logFile = nullptr;
		GUARANTEE_RECOVERABLE( false, "LogSystem: Can't open default log file!" );
	}

	// Hook IDE output tab
	LogHook( WriteToIDE, nullptr );

	// Start the log thread
	if( m_loggerThread == nullptr )
	{
		m_loggerThread = new std::thread( [](){ g_logSystem->LogThread( nullptr ); } );
	}
}

void LogSystem::LoggerShutdown()
{
	m_isRunning = false;

	// Join the logging thread
	if( m_loggerThread != nullptr )
	{
		m_loggerThread->join();

		delete m_loggerThread;
		m_loggerThread = nullptr;
	}

	// Close the log file
	if( g_logFile != nullptr )
	{
		// Write end of the HTML header tags
		std::string htmlEndTagStr = Stringf( "</body>\n</html>" );
		g_logFile->Write( htmlEndTagStr );

		// Close the file
		g_logFile->Close();
		delete g_logFile;
		g_logFile = nullptr;

		// Make a duplicate_timestamped file as a copy
		std::string timestamp			= GetCurrentTimestamp();
		std::string logHistoryFileName	= Stringf( "log_%s.html", timestamp.c_str() );
		std::string fullFilePath		= Stringf( "Log/%s", logHistoryFileName.c_str() );

		bool success = File::Copy( DEFAULT_LOG_FILE_PATH, fullFilePath.c_str() );
		GUARANTEE_RECOVERABLE( success, "LogSystem: Failed to copy log file in history folder!" );
	}
}

void LogSystem::LogHook( log_cb cb, void *userArg /* = nullptr */ )
{
	bool hookAlreadyExists = false;
	
	for( uint i = 0; i < m_hooks.GetSize(); i++ )
	{
		hookAlreadyExists = ( m_hooks.GetAtIndex(i).callback == cb ) && 
							( m_hooks.GetAtIndex(i).userArgument == userArg );
	}

	if( hookAlreadyExists == false )
	{
		LogHookData newHookData( cb, userArg );
		m_hooks.Add( newHookData );
	}
}

void LogSystem::LogUnhook( log_cb cb, void *userArg /* = nullptr */ )
{
	for( uint i = 0; i < m_hooks.GetSize(); i++ )
	{
		bool deleteThisHook =	( m_hooks.GetAtIndex(i).callback == cb ) &&
								( m_hooks.GetAtIndex(i).userArgument == userArg );

		if( deleteThisHook )
		{
			m_hooks.RemoveAtIndex(i);
			return;
		}
	}
}

void LogSystem::LogTaggedPrintv( char const *tag, Rgba const &color, char const *format, va_list args )
{
	std::string tagStr	= Stringf( tag );
	std::string textStr = Stringv( format, args );

	// Add data to the message queue
	LogData *newData = new LogData( tagStr, textStr, GetCurrentRawTime(), color );
	m_messageQueue.Enqueue( newData );
}

void LogSystem::LogTaggedPrintf( char const *tag, Rgba const &color, char const *format, ... )
{
	va_list args;
	va_start( args, format );

	LogTaggedPrintv( tag, color, format, args );

	va_end( args );
}

void LogSystem::LogPrintf( char const *format, ... )
{
	va_list args;
	va_start( args, format );

	LogTaggedPrintv( "log", RGBA_BLACK_COLOR, format, args );

	va_end( args );
}

void LogSystem::LogWarningf( char const *format, ... )
{
	va_list args;
	va_start( args, format );

	LogTaggedPrintv( "warning", RGBA_BLUE_COLOR, format, args );

	va_end( args );
}

void LogSystem::LogErrorf( char const *format, ... )
{
	va_list args;
	va_start( args, format );

	LogTaggedPrintv( "error", RGBA_RED_COLOR, format, args );

	va_end( args );
}

void LogSystem::ForceFlush()
{
	if( g_logSystem == nullptr )
		return;

	// Make sure you flush the message queue
	g_logSystem->FlushMessages();

	// Make sure everything is written to file
	g_logFile->Flush();
}

void LogSystem::GetAsStringFromLogData( std::string &out_logMessage, LogData const *logData )
{
	std::string timeStamp = GetTimeAsString( logData->time );
	out_logMessage = Stringf( "%s %s: %s\n", timeStamp.c_str(), logData->tag.c_str(), logData->text.c_str() );
}

void LogSystem::GetAsHTMLTagFromLogData( std::string &out_logMessage, LogData const *logData )
{
	Rgba const	&tagColor		= logData->color;
	std::string	 cssTagStr		= Stringf( "color: rgb(%i,%i,%i)", tagColor.r, tagColor.g, tagColor.b );
	std::string	 styleTagStr	= Stringf( "style=\"%s\"", cssTagStr.c_str() );
	std::string	 fontStartStr	= Stringf( "<font %s>", styleTagStr.c_str() );
	std::string	 fontEndStr		= Stringf( "</font>" );
	std::string	 logMessageStr;
	LogSystem::GetAsStringFromLogData( logMessageStr, logData );
	
	ReplaceAllInString( logMessageStr, "\n", "<br/>" );

	out_logMessage				= Stringf( "%s%s%s\n", fontStartStr.c_str(), logMessageStr.c_str(), fontEndStr.c_str() );
}

void LogSystem::ShowAllTags()
{
	// Convert the filter to black list
	m_IsFilterListBlack = true;
	
	// Empty the blacklist
	m_filterList.Clear();
}

void LogSystem::HideAllTags()
{
	// Convert the filter to white list
	m_IsFilterListBlack = false;

	// Empty the whitelist
	m_filterList.Clear();
}

void LogSystem::ShowTag( std::string const &tagName )
{
	if( m_IsFilterListBlack )
		RemoveTagFromFilterList( tagName );		// Remove	if Blacklist
	else
		AddTagToFilterList( tagName );			// Add		if Whitelist
}

void LogSystem::HideTag( std::string const &tagName )
{
	if( m_IsFilterListBlack )
		AddTagToFilterList( tagName );			// Add		if Blacklist
	else
		RemoveTagFromFilterList( tagName );		// Remove	if Whitelist
}

void LogSystem::AddTagToFilterList( std::string const &tagName )
{
	m_filterList.AddUnique( tagName );
}

void LogSystem::RemoveTagFromFilterList( std::string const &tagName )
{
	m_filterList.RemoveAll( tagName );
}

bool LogSystem::TagIsNotHidden( std::string const &tagName )
{
	bool tagIsInList = m_filterList.Contains( tagName );
	bool isWhiteList = !m_IsFilterListBlack;

	return (isWhiteList) ? (tagIsInList) : (!tagIsInList);
}

void LogSystem::LogThread( void * )
{
	while ( IsRunning() ) {
		FlushMessages(); 
		std::this_thread::sleep_for( std::chrono::milliseconds(10) ); // Better yet, you can use a signal or semaphore that signaled when a message is logged; 
	}
}

void LogSystem::FlushMessages()
{
	LogData *log = nullptr;

	while ( m_messageQueue.Dequeue(&log) ) 
	{
		for ( uint idx = 0; idx < m_hooks.GetSize(); idx++ )
		{
			LogHookData hook = m_hooks.GetAtIndex( idx );

			// call only if tag is not filtered out
			if( TagIsNotHidden( log->tag ) )
				hook.callback( log, hook.userArgument ); 
		}
		
		// free up the log;
		delete log; 
	}
}
