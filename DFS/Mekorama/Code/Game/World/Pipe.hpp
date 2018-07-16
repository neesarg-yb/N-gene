#pragma once
#include "Game/GameObject.hpp"

enum ePipeForwardDirection
{
	PIPE_DIR_WORLD_FRONT = 0,
	PIPE_DIR_WORLD_BACK,
	PIPE_DIR_WORLD_LEFT,
	PIPE_DIR_WORLD_RIGHT,
	PIPE_DIR_WORLD_UP,
	PIPE_DIR_WORLD_DOWN,
	NUM_PIPE_DIRECTIONS
};

class Pipe: public GameObject
{
public:
	 Pipe( Vector3 const &startPos, ePipeForwardDirection pipeForward, float length );
	~Pipe();

public:
	void Update( float deltaSeconds );
	void ObjectSelected();

public:
	static	Vector3 GetDirectionAsVector3	( ePipeForwardDirection pipeDir );
			Vector3 GetRotationFromForward	( ePipeForwardDirection forward );
};