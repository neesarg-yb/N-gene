#pragma once
#include <map>
#include <vector>
#include "EventSubsciption.hpp"

typedef std::vector< EventSubscription* >				EventSubscriptionList;
typedef std::map< std::string, EventSubscriptionList >	EventSubscriptionsMap;

class EventSystem
{
public:
	 EventSystem();
	~EventSystem();

private:
	EventSubscriptionsMap m_subscriptions;

public:
	void FireEvent( std::string const &eventName, NamedProperties &args );

	void SubscribeFunctionToEvent		( std::string const &eventName, EventFunctionCallbackPtr functionPtr );
	void UnsubscribeFunctionFromEvent	( std::string const &eventName, EventFunctionCallbackPtr functionPtr );

	template <typename T, typename METHOD>
		void SubscribeMethodToEvent( std::string const &eventName, T &object, METHOD methodPtr );
	
	template <typename T, typename METHOD>
		void UnsubscribeMethodFromEvent( std::string const &eventName, T &object, METHOD methodPtr );
};

template <typename T, typename METHOD>
void EventSystem::SubscribeMethodToEvent( std::string const &eventName, T &object, METHOD methodPtr )
{
	// Make sure the the methodPts's signature matches
	//		if not, the static_cast results in a compile time error!
	typedef bool (T::*EventObjectMethodCallbackPtr) (NamedProperties &args);
	EventObjectMethodCallbackPtr castedMethodPtr = static_cast< EventObjectMethodCallbackPtr >( methodPtr );

	// Add new subscription
	EventSubscriptionList &currentSubscribers = m_subscriptions[ eventName ];
	currentSubscribers.push_back( new EventObjectMethodSubscription<T>( object, castedMethodPtr ) );
}

template <typename T, typename METHOD>
void EventSystem::UnsubscribeMethodFromEvent( std::string const &eventName, T &object, METHOD methodPtr )
{
	// Make sure the the methodPts's signature matches
	//		if not, the static_cast results in a compile time error!
	typedef bool (T::*EventObjectMethodCallbackPtr) (NamedProperties &args);
	EventObjectMethodCallbackPtr castedMethodPtr = static_cast<EventObjectMethodCallbackPtr>(methodPtr);

	EventSubscriptionsMap::iterator it = m_subscriptions.find( eventName );
	if( it == m_subscriptions.end() )
		return;

	// For each subscribers of given event
	EventSubscriptionList &currentSubscibers = it->second;
	for( int i = 0; i < currentSubscibers.size(); i++ )
	{
		// Only if the class type is same
		EventObjectMethodSubscription<T> *thisObjectMethodSubscription = dynamic_cast< EventObjectMethodSubscription<T>* >( currentSubscibers[i] );
		if( thisObjectMethodSubscription == nullptr )
			continue;

		// Only if the object & method is the same
		bool isObjectSame		 = ( &thisObjectMethodSubscription->m_object == &object );
		bool isMethodPointerSame = (  thisObjectMethodSubscription->m_method == castedMethodPtr );
		if( isObjectSame == false || isMethodPointerSame == false )
			continue;

		// Delete the subscriber
		delete currentSubscibers[i];
		currentSubscibers[i] = nullptr;

		// Erase the entry
		currentSubscibers.erase( currentSubscibers.begin() + i );
		break;
	}
}
