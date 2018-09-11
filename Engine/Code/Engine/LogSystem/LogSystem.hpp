#pragma once
#include <string>
#include "Engine/Core/Rgba.hpp"
#include "Engine/LogSystem/SpinLock.hpp"
#include "Engine/LogSystem/ThreadSafeQueue.hpp"
#include "Engine/LogSystem/ThreadSafeVector.hpp"

#define DEFAULT_LOG_FILE_PATH ("Log/log.html")
#define DEFAULT_LOG_HISTORY_FOLDER ("Log/History/")

struct LogData;

typedef void ( *log_cb ) ( LogData const *log, void *argument );

struct LogData
{
	std::string tag;
	std::string text;
	Rgba		color;
	time_t		time;

	LogData( std::string &inTag, std::string &inText, time_t const &entryTime, Rgba const &textColor = RGBA_BLACK_COLOR )
	{
		tag		= inTag;
		text	= inText;
		color	= textColor;
		time	= entryTime;
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

	bool operator == ( LogHookData const &compareWith )
	{
		bool allMembersMatches =	( callback		== compareWith.callback ) &&
									( userArgument	== compareWith.userArgument );

		return allMembersMatches;
	}
};

class LogSystem
{
public:
	 LogSystem();
	~LogSystem();

private:
	bool							 m_isRunning	= false;
	std::thread						*m_loggerThread	= nullptr;
	ThreadSafeVector< LogHookData >	 m_hooks;
	ThreadSafeQueue< LogData* >		 m_messageQueue;

	// Filter list ( Blacklist / Whitelist )
	bool							 m_IsFilterListBlack = true;
	ThreadSafeVector< std::string >	 m_filterList;

public:
	static LogSystem* GetInstance();

public:
	// Startup & Shutdown
	void		LoggerStartup( char const *fileRootName = DEFAULT_LOG_FILE_PATH );
	void		LoggerShutdown();
	bool inline IsRunning() { return m_isRunning; }

	// HOOKING
	// the callback and the user arg. make a unique key to remove on
	void LogHook	( log_cb cb, void *userArg = nullptr ); 
	void LogUnhook	( log_cb cb, void *userArg = nullptr );
	
	// Logging Call
	void LogTaggedPrintv( char const *tag, Rgba const &color, char const *format, va_list args ); 
	void LogTaggedPrintf( char const *tag, Rgba const &color, char const *format, ... );

	// HELPERS
	void LogPrintf	( char const *format, ... ); 
	void LogWarningf( char const *format, ... ); 
	void LogErrorf	( char const *format, ... ); 

	static void ForceFlush();
	static void GetAsStringFromLogData	( std::string &out_logMessage, LogData const *logData );
	static void GetAsHTMLTagFromLogData	( std::string &out_logMessage, LogData const *logData );

	// Filter list
	void ShowAllTags();
	void HideAllTags();
	void ShowTag( std::string const &tagName );
	void HideTag( std::string const &tagName );

private:
	void AddTagToFilterList		( std::string const &tagName );
	void RemoveTagFromFilterList( std::string const &tagName );
	bool TagIsNotHidden( std::string const &tagName );

private:
	void LogThread( void * );
	void FlushMessages();
};