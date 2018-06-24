#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Renderable.hpp"

class Scene;

enum eGameObjectType
{
	GAME_OBJECT_TERRAIN = 0,
	GAME_OBJECT_TANK,
	GAME_OBJECT_BULLET,
	GAME_OBJECT_ENEMY,
	GAME_OBJECT_ENEMY_BASE,
	NUM_GAME_OBJECT_TYPES
};

class GameObject
{
public:
			 GameObject( eGameObjectType type );
	virtual ~GameObject() { };

public:
	eGameObjectType const	 m_type;
	Transform				 m_transform;
	Renderable				*m_renderable	= nullptr;

public:
	virtual void Update					( float deltaSeconds ) = 0;
	virtual void AddRenderablesToScene	( Scene &activeScene ) = 0;
};