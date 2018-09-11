#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"

enum eGameObjectType
{
	GAMEOBJECT_TYPE_BLOCK = 0,
	GAMEOBJECT_TYPE_PIPE,
	GAMEOBJECT_TYPE_ROBOT,
	GAMEOBJECT_TYPE_TOWER,
	NUM_GAMEOBJECT_TYPES
};

class GameObject
{
public:
			 GameObject( eGameObjectType const &type );
	virtual ~GameObject();

public:
	Transform				 m_transform;
	Renderable				*m_renderable	= nullptr;
	eGameObjectType const	 m_type;

private:
	// PickID Stuff
	uint m_pickID = INVALID_PICK_ID ;
public:
	void SetPickID( uint pickID );
	uint GetPickID() const;

public:
	virtual void Update( float deltaSeconds ) = 0;
	virtual void ObjectSelected() {};
	
public:
	static	uint		s_nextPickID;
	static	uint		GetNewPickID();
	static	GameObject*	GetFromPickID( uint pickID );

private:
	static	std::map< uint, GameObject* > s_pickIDPool;
};