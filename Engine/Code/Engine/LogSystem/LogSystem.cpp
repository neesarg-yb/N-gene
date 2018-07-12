#pragma once
#include "LogSystem.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <thread>

#include "Engine/Core/EngineCommon.hpp"

LogSystem	*g_logSystem	= nullptr;

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

	if( m_loggerThread == nullptr )
	{
		m_loggerThread = new std::thread( [](){ g_logSystem->LogThread( nullptr ); } );
	}
}

void LogSystem::LoggerShutdown()
{
	m_isRunning = false;

	if( m_loggerThread != nullptr )
	{
		m_loggerThread->join();

		delete m_loggerThread;
		m_loggerThread = nullptr;
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

void LogSystem::LogThread( void * )
{
	while ( IsRunning() ) {
		FlushMessages(); 
		Sleep(10); // Better yet, you can use a signal or semaphore that signaled when a message is logged; 
	}
}

void LogSystem::FlushMessages()
{
	LogData *log;

	while ( m_messageQueue.Dequeue(&log) ) 
	{
		m_hookLock.Enter();
		for each (LogHookData hook in m_hooks) 
		{
			hook.callback( *log, hook.userArgument ); 
		}
		m_hookLock.Leave(); 

		// free up the log;
		delete log; 
	}
}
