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

struct LogHook
{
	log_cb	 callback;
	void	*userArgument;
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
	std::vector< LogHook >		 m_hooks;
	ThreadSafeQueue< LogData* >	 m_messageQueue;

public:
	static LogSystem* GetInstance();

public:
	// Startup & Shutdown
	void		LoggerStartup( char const *fileRootName = DEFAULT_LOG_NAME );
	void		LoggerShutdown();
	bool inline IsRunning() { return m_isRunning; }

private:
	void LogThread( void * );
	void FlushMessages();
};