#pragma once
#include "GameObject.hpp"

std::map< uint, GameObject* > GameObject::s_pickIDPool;

GameObject::GameObject( eGameObjectType const &type )
	: m_type( type )
{

}

GameObject::~GameObject()
{
	// If GameObject was in PickID pool, remove it
	if( m_pickID != INVALID_PICK_ID )
		s_pickIDPool.erase( m_pickID );
}

void GameObject::SetPickID( uint pickID )
{
	m_pickID = pickID;

	// If GameObject care about its PickID, add it to pool
	if( m_pickID != INVALID_PICK_ID )
		s_pickIDPool[ pickID ] = this;
}

uint GameObject::GetPickID() const
{
	return m_pickID;
}

uint GameObject::s_nextPickID = 1U;

uint GameObject::GetNewPickID()
{
	return s_nextPickID++;
}

GameObject* GameObject::GetFromPickID( uint pickID )
{
	std::map< uint, GameObject* >::iterator it = s_pickIDPool.find( pickID );
	
	bool isGameObjectInPool	= ( it != s_pickIDPool.end() );
	GameObject*	toReturn	= isGameObjectInPool ? s_pickIDPool[pickID] : nullptr;

	return toReturn;
}
