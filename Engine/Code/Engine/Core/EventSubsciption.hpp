#pragma once
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/EngineCommon.hpp"

class EventSystem;
class EventSubscription
{
	friend EventSystem;

public:
			 EventSubscription() { }
	virtual	~EventSubscription() { }

public:
	virtual bool Execute( NamedProperties& args ) = 0;
};

class EventFunctionSubscription : public EventSubscription
{
	friend EventSystem;

public:
	 EventFunctionSubscription( EventFunctionCallbackPtr registerFunction ) : m_function( registerFunction ) { }
	~EventFunctionSubscription() { }

private:
	EventFunctionCallbackPtr m_function = nullptr;

public:
	bool Execute( NamedProperties& args );
};
