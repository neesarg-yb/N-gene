#pragma once
#include <string>
#include "Engine/LogSystem/SpinLock.hpp"
#include "Engine/LogSystem/ThreadSafeQueue.hpp"

#define DEFAULT_LOG_NAME ("systemLog.txt")

struct LogData;

typedef void ( *log_cb ) ( LogData const *log, void *argument );

struct LogData
{
	std::string tag;
	std::string text;

	LogData( std::string &inTag, std::string &inText )
	{
		tag		= inTag;
		text	= inText;
	}
};

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
	
	// Logging Call (Master call - most functions will feed through this)
	// Notice we take a va_list, so we can forward other functions that
	// take variable arguments to this; 
	void LogTaggedPrintv( char const *tag, char const *format, va_list args ); 
	void LogTaggedPrintf( char const *tag, char const *format, ... ); 

private:
	void LogThread( void * );
	void FlushMessages();
};