#pragma once
#include "Logger.hpp"
#include <thread>

std::thread *loggerThread = nullptr;

void LoggerStartup( char const *fileRootName /*= DEFAULT_LOG_NAME */ )
{
	if( loggerThread == nullptr )
	{
		loggerThread = new std::thread();
	}
}

void LoggerShutdown()
{
	if( loggerThread != nullptr )
	{
		loggerThread->join();

		delete loggerThread;
		loggerThread = nullptr;
	}
}
