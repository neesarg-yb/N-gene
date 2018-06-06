#pragma once
#include "Game/GameObject.hpp"

class Tank : public GameObject
{
public:
	 Tank( Vector3 const &spawnPosition );
	~Tank();

public:
	void Update( float deltaSeconds );
};