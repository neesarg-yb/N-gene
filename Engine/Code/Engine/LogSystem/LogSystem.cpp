#pragma once
#include "LogSystem.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <thread>

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/File/File.hpp"

LogSystem	*g_logSystem	= nullptr;
File		*g_logFile		= nullptr;

void WriteToFile( LogData const *logData, void *filePointer )
{
	File *fp = (File*) filePointer;
	
	std::string logStr = Stringf( "%s: %s\n", logData->tag.c_str(), logData->text.c_str() );
	fp->Write( logStr );
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
	
	for( uint i = 0; i < m_hooks.size(); i++ )
	{
		hookAlreadyExists = ( m_hooks[i].callback == cb ) && 
							( m_hooks[i].userArgument == userArg );
	}

	if( hookAlreadyExists == false )
	{
		LogHookData newHookData( cb, userArg );
		m_hooks.push_back( newHookData );
	}
}

void LogSystem::LogUnhook( log_cb cb, void *userArg /* = nullptr */ )
{
	for( uint i = 0; i < m_hooks.size(); i++ )
	{
		bool deleteThisHook =	( m_hooks[i].callback == cb ) &&
								( m_hooks[i].userArgument == userArg );

		if( deleteThisHook )
		{
			// Fast remove
			std::swap( m_hooks[i], m_hooks.back() );
			m_hooks.pop_back();

			return;
		}
	}
}

void LogSystem::LogTaggedPrintv( char const *tag, char const *format, va_list args )
{
	std::string tagStr	= Stringf( tag );
	std::string textStr = Stringf( format, args );

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

void LogSystem::LogThread( void * )
{
	while ( IsRunning() ) {
		FlushMessages(); 
		Sleep(10); // Better yet, you can use a signal or semaphore that signaled when a message is logged; 
	}
}

void LogSystem::FlushMessages()
{
	LogData *log = nullptr;

	while ( m_messageQueue.Dequeue(&log) ) 
	{
		m_hookLock.Enter();
		for each (LogHookData hook in m_hooks) 
		{
			hook.callback( log, hook.userArgument ); 
		}
		m_hookLock.Leave(); 

		// free up the log;
		delete log; 
	}
}
