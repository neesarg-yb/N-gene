#pragma once
#include <string>

#define DEFAULT_LOG_NAME ("log")

struct LogData
{
	std::string tag;
	std::string text;
};

typedef void ( *log_cb ) ( LogData const &log );

// Startup & Shutdown
void LoggerStartup( char const *fileRootName = DEFAULT_LOG_NAME );
void LoggerShutdown();