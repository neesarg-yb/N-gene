#pragma once
#include <map>
#include <vector>
#include "EventSubsciption.hpp"

typedef std::vector< EventSubscription* >				EventSubscriptionList;
typedef std::map< std::string, EventSubscriptionList >	EventSubscriptionsMap;

class EventSystem
{
public:
	 EventSystem() { }
	~EventSystem() { }

private:
	EventSubscriptionsMap m_subscriptions;

public:
	void FireEvent( std::string const &eventName, NamedProperties &args );

	void SubscribeFunctionToEvent		( std::string const &eventName, EventFunctionCallbackPtr functionPtr );
	void UnsubscribeFunctionFromEvent	( std::string const &eventName, EventFunctionCallbackPtr functionPtr );
};
