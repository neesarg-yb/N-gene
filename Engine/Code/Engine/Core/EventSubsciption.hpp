#pragma once
#include "Engine/Core/NamedProperties.hpp"

typedef bool ( *EventFunctionCallbackPtr ) (NamedProperties& args);

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
	bool Execute( NamedProperties& args ) override;
};

template <typename T>
class EventObjectMethodSubscription : public EventSubscription
{
	friend EventSystem;
	typedef bool (T::*EventObjectMethodCallbackPtr) (NamedProperties &args );

private:
	T &m_object;
	EventObjectMethodCallbackPtr m_method	= nullptr;

public:
	 EventObjectMethodSubscription( T &objectPtr, EventObjectMethodCallbackPtr registerMethod ) : m_object( objectPtr ), m_method( registerMethod ) { }
	~EventObjectMethodSubscription() { }

public:
	bool Execute( NamedProperties& args ) override;
};

template <typename T>
bool EventObjectMethodSubscription<T>::Execute( NamedProperties& args )
{
	return (m_object.*m_method)( args );
}
