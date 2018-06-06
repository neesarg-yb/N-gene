#pragma once
#include "MenuAction.hpp"

MenuAction::MenuAction( const char* actionName, action_cb* callbackFunction /* = nullptr */ )
	: m_actionName( actionName )
	, m_callbackFunction( callbackFunction )
{

}

MenuAction::~MenuAction()
{

}