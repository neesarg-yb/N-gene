#pragma once
#include "EventSystem.hpp"

void EventSystem::FireEvent( std::string const &eventName, NamedProperties &args )
{
	// Find the event
	EventSubscriptionsMap::iterator it = m_subscriptions.find( eventName );
	if( it == m_subscriptions.end() )
		return;

	// Do a callback to each subscribers
	EventSubscriptionList subscribersCopy = it->second;
	for( EventSubscription* sub : subscribersCopy )
		sub->Execute( args );
}

void EventSystem::SubscribeFunctionToEvent( std::string const &eventName, EventFunctionCallbackPtr functionPtr )
{
	TODO( "Avoid Duplicate Entries" );

	EventSubscriptionList &currentSubscribers = m_subscriptions[ eventName ];
	currentSubscribers.push_back( new EventFunctionSubscription( functionPtr ) );
}

void EventSystem::UnsubscribeFunctionFromEvent( std::string const &eventName, EventFunctionCallbackPtr functionPtr )
{
	EventSubscriptionsMap::iterator it = m_subscriptions.find( eventName );
	if( it == m_subscriptions.end() )
		return;

	// For each subscribers of given event
	EventSubscriptionList &currentSubscibers = it->second;
	for( int i = 0; i < currentSubscibers.size(); i++ )
	{
		// Only if the type is same
		EventFunctionSubscription *thisFunctionSubscription = dynamic_cast< EventFunctionSubscription* >( currentSubscibers[i] );
		if( thisFunctionSubscription == nullptr )
			continue;

		// Only if the pointer location is same
		if( thisFunctionSubscription->m_function != functionPtr )
			continue;

		// Delete the subscriber
		delete currentSubscibers[i];
		currentSubscibers[i] = nullptr;

		// Erase the entry
		currentSubscibers.erase( currentSubscibers.begin() + i );
		break;
	}
}
