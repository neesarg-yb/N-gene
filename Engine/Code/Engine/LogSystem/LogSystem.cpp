#pragma once
#include "LogSystem.hpp"

#ifdef _WIN32
#define PLATFORM_WINDOWS
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <thread>
#include <iostream>

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/File/File.hpp"

LogSystem	*g_logSystem	= nullptr;
File		*g_logFile		= nullptr;

void WriteToFile( LogData const *logData, void *filePointer )
{
	File *fp = (File*) filePointer;
	
	std::string logStr;
	LogSystem::GetFormattedMessageFromLogData( logStr, logData );

	fp->Write( logStr );
}

void WriteToIDE( LogData const *logData, void* )
{
	// Get log message as string
	std::string messageStr;
	LogSystem::GetFormattedMessageFromLogData( messageStr, logData );

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
		LogHook( WriteToFile, g_logFile );
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
		g_logFile->Close();
		delete g_logFile;
		g_logFile = nullptr;
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

void LogSystem::LogTaggedPrintv( char const *tag, char const *format, va_list args )
{
	std::string tagStr	= Stringf( tag );
	std::string textStr = Stringv( format, args );

	// Add data to the message queue
	LogData *newData = new LogData( tagStr, textStr );
	m_messageQueue.Enqueue( newData );
}

void LogSystem::LogTaggedPrintf( char const *tag, char const *format, ... )
{
	va_list args;
	va_start( args, format );

	LogTaggedPrintv( tag, format, args );

	va_end( args );
}

void LogSystem::LogPrintf( char const *format, ... )
{
	va_list args;
	va_start( args, format );

	LogTaggedPrintv( "log", format, args );

	va_end( args );
}

void LogSystem::LogWarningf( char const *format, ... )
{
	va_list args;
	va_start( args, format );

	LogTaggedPrintv( "warning", format, args );

	va_end( args );
}

void LogSystem::LogErrorf( char const *format, ... )
{
	va_list args;
	va_start( args, format );

	LogTaggedPrintv( "error", format, args );

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

void LogSystem::GetFormattedMessageFromLogData( std::string &out_logMessage, LogData const *logData )
{
	out_logMessage = Stringf( "%s: %s\n", logData->tag.c_str(), logData->text.c_str() );
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
			hook.callback( log, hook.userArgument ); 
		}
		
		// free up the log;
		delete log; 
	}
}
