#pragma once
#include "Game/GameObject.hpp"

class Robot: public GameObject
{
public:
	 Robot( Vector3 const &position );
	~Robot();

public:
	void Update( float deltaSeconds );
};