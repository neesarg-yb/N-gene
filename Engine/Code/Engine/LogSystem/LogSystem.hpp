#pragma once
#include <string>
#include "Engine/LogSystem/SpinLock.hpp"
#include "Engine/LogSystem/ThreadSafeQueue.hpp"

#define DEFAULT_LOG_NAME ("log")

struct LogData
{
	std::string tag;
	std::string text;
};

typedef void ( *log_cb ) ( LogData const &log, void *argument );

struct LogHookData
{
	log_cb	 callback;
	void	*userArgument;

	LogHookData( log_cb cb, void *userArg )
	{
		callback		= cb;
		userArgument	= userArg;
	}
};


class LogSystem
{
public:
	 LogSystem();
	~LogSystem();

private:
	bool						 m_isRunning	= false;
	std::thread					*m_loggerThread	= nullptr;
	SpinLock					 m_hookLock;
	std::vector< LogHookData >	 m_hooks;
	ThreadSafeQueue< LogData* >	 m_messageQueue;

public:
	static LogSystem* GetInstance();

public:
	// Startup & Shutdown
	void		LoggerStartup( char const *fileRootName = DEFAULT_LOG_NAME );
	void		LoggerShutdown();
	bool inline IsRunning() { return m_isRunning; }

	// HOOKING
	// the callback and the user arg. make a unique key to remove on
	void LogHook	( log_cb cb, void *userArg = nullptr ); 
	void LogUnhook	( log_cb cb, void *userArg = nullptr );

private:
	void LogThread( void * );
	void FlushMessages();
};