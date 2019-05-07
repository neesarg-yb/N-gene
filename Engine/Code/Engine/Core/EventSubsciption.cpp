#pragma once
#include "EventSubsciption.hpp"

bool EventFunctionSubscription::Execute( NamedProperties& args )
{
	return m_function( args );
}

