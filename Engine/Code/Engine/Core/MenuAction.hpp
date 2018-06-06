#pragma once
#include <string>
#include <functional>

typedef std::function< void( const char* ) > action_cb;

class MenuAction
{
public:
	 MenuAction( const char* actionName, action_cb* callbackFunction = nullptr );
	~MenuAction();

	const std::string	m_actionName;
	action_cb* const	m_callbackFunction	=	nullptr;

private:

};