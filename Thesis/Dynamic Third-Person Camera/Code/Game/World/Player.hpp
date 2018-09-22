#pragma once
#include "Game/World/GameObject.hpp"

class Terrain;

class Player : public GameObject
{
public:
	 Player( Vector3 worldPosition, Terrain const &parentTerrain );
	~Player();

public:
	// What a player will do?
	// (1) Roam Around the Terrain
	//		-> Needs a pointer to Terrain
	// (2) Gets velocity according to input
	//		-> Goes in Update()
	// (3) Jump
	//		-> Y-Velocity & Gravity
	// (4) It is a sphere for now
	//		-> Renderable
	Terrain const *m_terrain = nullptr;			// The terrain this player is on

public:
	void Update( float deltaSeconds );
	void AddRenderablesToScene( Scene &activeScene );
	void RemoveRenderablesFromScene( Scene &activeScene );
};