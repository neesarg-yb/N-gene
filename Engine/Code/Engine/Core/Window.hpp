#pragma once

#include <vector>
#include "Engine/Core/EngineCommon.hpp"

typedef bool (*windows_message_handler_cb)( unsigned int msg, size_t wparam, size_t lparam ); 

class Window
{
private:
	Window( char const *title, float clientAspect );

public:
	~Window(); 

	void*	GetHandle() const { return m_hwnd; }
	void	AddMessageHandler	( windows_message_handler_cb cb ); 
	void	RemoveMessageHandler( windows_message_handler_cb cb );
	
	unsigned int	GetWidth () const;
	unsigned int	GetHeight() const;

public:
	void*	m_hwnd; 
	std::vector<windows_message_handler_cb> m_message_handlers; 

public:
	static Window* CreateInstance( char const *title, float aspect );
	static Window* GetInstance();
}; 

